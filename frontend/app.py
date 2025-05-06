import streamlit as st
import requests
from datetime import datetime
import pytz
import json
import time

# Constants
API_BASE_URL = "http://localhost:8000/api"
WASTE_TYPES = ["PLASTIC", "ELECTRONIC", "ORGANIC", "HAZARDOUS"]
STATUS_TYPES = ["PENDING", "SCHEDULED", "COMPLETED"]

# Page configuration
st.set_page_config(
    page_title="WiseWaste Management",
    page_icon="♻️",
    layout="wide"
)

# Initialize session state
if 'username' not in st.session_state:
    st.session_state.username = ""
if 'backend_status' not in st.session_state:
    st.session_state.backend_status = "unknown"

def check_backend_connection():
    try:
        response = requests.get(f"{API_BASE_URL}/wastepickups", timeout=2)
        if response.status_code == 200:
            st.session_state.backend_status = "connected"
            return True
        else:
            st.session_state.backend_status = "error"
            return False
    except requests.exceptions.RequestException:
        st.session_state.backend_status = "disconnected"
        return False

def create_pickup_request(data):
    try:
        headers = {
            'Content-Type': 'application/json',
            'Accept': 'application/json'
        }
        json_str = json.dumps(data)
        st.write("Sending request with data:", json_str)
        
        response = requests.post(
            f"{API_BASE_URL}/wastepickups",
            data=json_str,
            headers=headers,
            timeout=5
        )
        st.write("Response status code:", response.status_code)
        st.write("Response headers:", dict(response.headers))
        
        # Show raw response for debugging
        st.write("Raw response text:", response.text)
        
        if response.ok:
            if response.text:
                try:
                    # Try to parse the response as JSON
                    json_response = json.loads(response.text)
                    return response.status_code == 201, json_response
                except json.JSONDecodeError as e:
                    # If the response is not JSON but status is OK, consider it successful
                    if response.status_code == 201:
                        return True, {"message": "Request successful", "raw_response": response.text}
                    st.error(f"Invalid JSON in response: {str(e)}")
                    return False, None
            else:
                # Empty response with success status
                if response.status_code == 201:
                    return True, {"message": "Request successful"}
                return False, None
        else:
            st.error(f"Server returned error status: {response.status_code}")
            return False, None
    except requests.exceptions.RequestException as e:
        st.error(f"Connection error: {str(e)}")
        return False, None

def get_all_pickups():
    try:
        response = requests.get(f"{API_BASE_URL}/wastepickups", timeout=5)
        if not response.ok:
            st.warning(f"Server returned status code: {response.status_code}")
            return []
        
        # Try to parse the JSON, handle empty responses
        try:
            return response.json()
        except json.JSONDecodeError as e:
            st.error(f"Failed to parse JSON response: {str(e)}")
            if response.text:
                st.code(response.text[:200] + "..." if len(response.text) > 200 else response.text)
            else:
                st.warning("Server returned empty response")
            return []
    except requests.exceptions.RequestException as e:
        st.error(f"Connection error: {str(e)}")
        return []

def delete_pickup(pickup_id):
    try:
        response = requests.delete(f"{API_BASE_URL}/wastepickups/{pickup_id}", timeout=5)
        return response.status_code == 204
    except requests.exceptions.RequestException as e:
        st.error(f"Connection error: {str(e)}")
        return False

def update_pickup(pickup_id, data):
    try:
        response = requests.put(f"{API_BASE_URL}/wastepickups/{pickup_id}", json=data, timeout=5)
        return response.status_code == 200, response.json() if response.ok else None
    except requests.exceptions.RequestException as e:
        st.error(f"Connection error: {str(e)}")
        return False, None
    except json.JSONDecodeError:
        st.error("Response is not valid JSON")
        return False, None

# Main UI
st.title("♻️ WiseWaste Management System")

# Check backend connection
backend_connected = check_backend_connection()
if st.session_state.backend_status == "disconnected":
    st.error("⚠️ Cannot connect to backend server. Please make sure it's running at " + API_BASE_URL)
    if st.button("Try again"):
        st.experimental_rerun()
    st.stop()
elif st.session_state.backend_status == "error":
    st.warning("⚠️ Backend server is reachable but returned an error.")

# Username input
if not st.session_state.username:
    st.session_state.username = st.text_input("Enter your username to continue:")
    if not st.session_state.username:
        st.stop()

# Tabs for different functionalities
tab1, tab2 = st.tabs(["Submit Pickup Request", "View & Manage Pickups"])

# Submit Pickup Request Tab
with tab1:
    st.header("Submit New Pickup Request")
    
    with st.form("pickup_form"):
        waste_type = st.selectbox("Waste Type", WASTE_TYPES)
        pickup_location = st.text_input("Pickup Location")
        pickup_date = st.date_input("Pickup Date")
        pickup_time = st.time_input("Pickup Time")
        
        submit_button = st.form_submit_button("Submit Request")
        
        if submit_button:
            if not pickup_location:
                st.error("Please enter a pickup location")
            else:
                # Combine date and time
                pickup_datetime = datetime.combine(pickup_date, pickup_time)
                pickup_datetime = pickup_datetime.replace(tzinfo=pytz.UTC)
                
                data = {
                    "wasteType": waste_type,
                    "pickupLocation": pickup_location,
                    "pickupDateTime": pickup_datetime.strftime("%Y-%m-%d %H:%M:%S"),
                    "status": "PENDING",
                    "userName": st.session_state.username
                }
                
                # Debug: Show the data being sent
                st.write("Preparing to send request with data:")
                st.json(data)
                
                # Convert to JSON string to verify format
                json_str = json.dumps(data, indent=2)
                st.code(json_str, language="json")
                
                # Make the request with headers
                try:
                    headers = {
                        'Content-Type': 'application/json',
                        'Accept': 'application/json'
                    }
                    response = requests.post(
                        f"{API_BASE_URL}/wastepickups",
                        data=json_str,
                        headers=headers,
                        timeout=5
                    )
                    
                    st.write("Response status code:", response.status_code)
                    st.write("Response headers:", dict(response.headers))
                    
                    # Show raw response for debugging
                    st.write("Raw response text:")
                    st.code(response.text)
                    
                    if response.ok:
                        if response.text:  # Check if response has content
                            try:
                                json_response = response.json()
                                st.success("Pickup request submitted successfully!")
                                st.write("Server response:", json_response)
                            except json.JSONDecodeError as e:
                                st.error(f"Server returned invalid JSON: {str(e)}")
                                st.write("Raw response:", response.text)
                        else:
                            st.warning("Server returned empty response with status 200")
                    else:
                        st.error(f"Server returned error status: {response.status_code}")
                        st.write("Error response:", response.text)
                except requests.exceptions.RequestException as e:
                    st.error(f"Connection error: {str(e)}")
                except Exception as e:
                    st.error(f"Unexpected error: {str(e)}")

# View & Manage Pickups Tab
with tab2:
    st.header("Your Pickup Requests")
    
    # Add refresh button
    if st.button("Refresh List"):
        st.experimental_rerun()
    
    # Get all pickups
    pickups = get_all_pickups()
    
    # Filter pickups for current user
    user_pickups = [p for p in pickups if p["userName"] == st.session_state.username]
    
    if not user_pickups:
        st.info("You have no pickup requests.")
    else:
        # Create three columns for different status types
        cols = st.columns(3)
        status_columns = {
            "PENDING": cols[0],
            "SCHEDULED": cols[1],
            "COMPLETED": cols[2]
        }
        
        # Group pickups by status
        for status in STATUS_TYPES:
            with status_columns[status]:
                st.subheader(status)
                status_pickups = [p for p in user_pickups if p["status"] == status]
                
                for pickup in status_pickups:
                    with st.expander(f"{pickup['wasteType']} - {pickup['pickupLocation']}"):
                        st.write(f"ID: {pickup['id']}")
                        st.write(f"Location: {pickup['pickupLocation']}")
                        st.write(f"Date & Time: {pickup['pickupDateTime']}")
                        
                        # Show actions only for non-completed pickups
                        if pickup["status"] != "COMPLETED":
                            # Cancel button
                            if st.button("Cancel Request", key=f"cancel_{pickup['id']}"):
                                if delete_pickup(pickup['id']):
                                    st.success("Pickup request cancelled successfully!")
                                    st.experimental_rerun()
                                else:
                                    st.error("Failed to cancel pickup request.")
                            
                            # Reschedule form
                            with st.form(f"reschedule_{pickup['id']}"):
                                st.write("Reschedule Pickup")
                                new_date = st.date_input("New Date", key=f"date_{pickup['id']}")
                                new_time = st.time_input("New Time", key=f"time_{pickup['id']}")
                                
                                if st.form_submit_button("Reschedule"):
                                    new_datetime = datetime.combine(new_date, new_time)
                                    new_datetime = new_datetime.replace(tzinfo=pytz.UTC)
                                    
                                    update_data = pickup.copy()
                                    update_data["pickupDateTime"] = new_datetime.strftime("%Y-%m-%d %H:%M:%S")
                                    
                                    success, _ = update_pickup(pickup['id'], update_data)
                                    if success:
                                        st.success("Pickup rescheduled successfully!")
                                        st.experimental_rerun()
                                    else:
                                        st.error("Failed to reschedule pickup.") 