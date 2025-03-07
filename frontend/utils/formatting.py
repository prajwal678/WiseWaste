import pandas as pd
import streamlit as st
from datetime import datetime


def format_datetime(datetime_str):
    """Format datetime string to a more readable format"""
    try:
        dt = datetime.strptime(datetime_str, "%Y-%m-%d %H:%M:%S")
        return dt.strftime("%b %d, %Y at %I:%M %p")
    except:
        return datetime_str


def get_status_color(status):
    """Return color based on pickup status"""
    if status == "Completed":
        return "green"
    elif status == "Scheduled":
        return "blue"
    else:  # Pending
        return "orange"


def get_waste_type_icon(waste_type):
    """Return emoji icon based on waste type"""
    if waste_type == "Plastic":
        return "♻️"
    elif waste_type == "Electronic":
        return "🖥️"
    elif waste_type == "Hazardous":
        return "☢️"
    elif waste_type == "Organic":
        return "🌱"
    else:
        return "🗑️"


def create_pickup_df(pickups_data):
    """Convert pickups data to DataFrame for display"""
    if not pickups_data or isinstance(pickups_data, dict) and "error" in pickups_data:
        return pd.DataFrame()

    df = pd.DataFrame(pickups_data)

    # Rename columns for better display
    if len(df) > 0:
        df = df.rename(columns={
            "id": "ID",
            "wasteType": "Waste Type",
            "pickupLocation": "Location",
            "pickupDateTime": "Scheduled Time",
            "status": "Status",
            "userName": "User",
            "creationTimestamp": "Created At"
        })

        # Format datetime columns
        if "Scheduled Time" in df.columns:
            df["Scheduled Time"] = df["Scheduled Time"].apply(format_datetime)
        if "Created At" in df.columns:
            df["Created At"] = df["Created At"].apply(format_datetime)

    return df


def display_pickup_card(pickup):
    """Display a single pickup as a card"""
    status_color = get_status_color(pickup.get("status", "Pending"))
    waste_icon = get_waste_type_icon(pickup.get("wasteType", ""))

    st.markdown(f"""
    <div style="border:1px solid #ddd; border-radius:10px; padding:15px; margin-bottom:15px; border-left:5px solid {status_color};">
        #{pickup.get("id", "N/A")} - {pickup.get("wasteType", "Unknown")} Waste</h3>
        <h3>{waste_icon} Pickup
        <p><strong>Location:</strong> {pickup.get("pickupLocation", "Unknown")}</p>
        <p><strong>Scheduled:</strong> {format_datetime(pickup.get("pickupDateTime", ""))}</p>
        <p><strong>Status:</strong> <span style="color:{status_color}; font-weight:bold;">{pickup.get("status", "Pending")}</span></p>
        <p><strong>Created:</strong> {format_datetime(pickup.get("creationTimestamp", ""))}</p>
    </div>
    """, unsafe_allow_html=True)
