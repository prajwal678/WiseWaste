# WiseWaste

This is the frontend application for the WiseWaste Management System, built using Streamlit.

## Prerequisites

- Python 3.7 or higher
- pip (Python package installer)
- Backend server running on localhost:8000

## Installation

1. Create a virtual environment (recommended):
   ```bash
   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```

2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```

## Running the Application

1. Make sure the backend server is running on localhost:8000

2. Start the Streamlit application:
   ```bash
   streamlit run app.py
   ```

3. Open your browser and navigate to http://localhost:8501

## Features

- Submit new waste pickup requests
- View all your pickup requests organized by status
- Cancel pending or scheduled pickups
- Reschedule pickup times
- Real-time status updates
- User-specific request management

## Usage

1. Enter your username when prompted
2. Use the tabs to navigate between different functionalities:
   - "Submit Pickup Request" tab for creating new requests
   - "View & Manage Pickups" tab for managing existing requests
3. All pickup requests are automatically marked as completed when their scheduled time has passed 