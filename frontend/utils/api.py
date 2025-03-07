import requests
import json
from datetime import datetime

BASE_URL = "http://localhost:8000/api"


def get_all_pickups():
    try:
        response = requests.get(f"{BASE_URL}/pickups")
        if response.status_code == 200:
            return response.json()
        else:
            return {"error": f"Failed to fetch pickups: {response.status_code}"}
    except Exception as e:
        return {"error": f"Error: {str(e)}"}


def get_pickup_by_id(pickup_id):
    try:
        response = requests.get(f"{BASE_URL}/pickups/{pickup_id}")
        if response.status_code == 200:
            return response.json()
        else:
            return {"error": f"Failed to fetch pickup: {response.status_code}"}
    except Exception as e:
        return {"error": f"Error: {str(e)}"}


def create_pickup(waste_type, pickup_location, pickup_date_time, user_name):
    try:
        payload = {
            "waste_type": waste_type,
            "pickup_location": pickup_location,
            "pickup_date_time": pickup_date_time,
            "user_name": user_name,
            "status": "pending"
        }

        response = requests.post(
            f"{BASE_URL}/pickups",
            json=payload,
            headers={"Content-Type": "application/json"}
        )

        if response.status_code == 200 or response.status_code == 201:
            return {"success": True, "data": response.json()}
        else:
            return {"success": False, "error": f"Failed to create pickup: {response.status_code} - {response.text}"}
    except Exception as e:
        return {"success": False, "error": f"Error: {str(e)}"}


def update_pickup(pickup_id, waste_type, pickup_location, pickup_date_time, status, user_name):
    try:
        payload = {
            "waste_type": waste_type,
            "pickup_location": pickup_location,
            "pickup_date_time": pickup_date_time,
            "status": status,
            "user_name": user_name
        }

        response = requests.put(
            f"{BASE_URL}/pickups/{pickup_id}",
            json=payload,
            headers={"Content-Type": "application/json"}
        )

        if response.status_code == 200:
            return {"success": True, "data": response.json()}
        else:
            return {"success": False, "error": f"Failed to update pickup: {response.status_code} - {response.text}"}
    except Exception as e:
        return {"success": False, "error": f"Error: {str(e)}"}


def delete_pickup(pickup_id):
    try:
        response = requests.delete(f"{BASE_URL}/pickups/{pickup_id}")
        if response.status_code == 200:
            return {"success": True}
        else:
            return {"success": False, "error": f"Failed to delete pickup: {response.status_code} - {response.text}"}
    except Exception as e:
        return {"success": False, "error": f"Error: {str(e)}"}


def get_environmental_impact():
    try:
        response = requests.get(f"{BASE_URL}/environmental-impact")
        if response.status_code == 200:
            return response.json()
        else:
            return {"error": f"Failed to fetch environmental impact data: {response.status_code}"}
    except Exception as e:
        return {"error": f"Error: {str(e)}"}
