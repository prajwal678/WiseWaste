import streamlit as st
from datetime import datetime, timedelta
from utils.api import create_pickup


def show_request_pickup():
    st.markdown("<h1 class='main-header'>Request Waste Pickup</h1>",
                unsafe_allow_html=True)

    # Form for creating new pickup request
    with st.form("pickup_request_form"):
        # Waste type selection
        waste_type = st.selectbox(
            "Waste Type",
            options=["plastic", "electronic", "hazardous", "organic"],
            help="Select the type of waste for pickup"
        )

        # Pickup location
        pickup_location = st.text_input(
            "Pickup Location",
            help="Enter the address or location for waste pickup"
        )

        # Date and time selection
        col1, col2 = st.columns(2)

        with col1:
            # Min date: today, Max date: 30 days from now
            min_date = datetime.now().date()
            max_date = min_date + timedelta(days=30)

            pickup_date = st.date_input(
                "Pickup Date",
                min_value=min_date,
                max_value=max_date,
                value=min_date + timedelta(days=1),
                help="Select preferred pickup date"
            )

        with col2:
            pickup_time = st.time_input(
                "Pickup Time",
                value=datetime.now().replace(hour=9, minute=0, second=0).time(),
                help="Select preferred pickup time"
            )

        # Combine date and time
        pickup_datetime = f"{pickup_date} {pickup_time}"

        # Special instructions (optional)
        special_instructions = st.text_area(
            "Special Instructions (Optional)",
            help="Add any special instructions for the pickup"
        )

        # Submit button
        submit_button = st.form_submit_button("Schedule Pickup")

    # Handle form submission
    if submit_button:
        if not pickup_location:
            st.error("Pickup location is required.")
        else:
            # Format datetime string for API
            pickup_dt_obj = datetime.strptime(
                f"{pickup_date} {pickup_time}", "%Y-%m-%d %H:%M:%S")
            formatted_datetime = pickup_dt_obj.strftime("%Y-%m-%d %H:%M:%S")

            # Add special instructions to location if provided
            full_location = pickup_location
            if special_instructions:
                full_location += f" (Note: {special_instructions})"

            # Create pickup request via API
            result = create_pickup(
                waste_type=waste_type,
                pickup_location=full_location,
                pickup_date_time=formatted_datetime,
                user_name=st.session_state.user_name
            )

            if result.get("success", False):
                st.success("Pickup scheduled successfully!")
                st.balloons()

                # Show confirmation details
                st.markdown("### Pickup Details")
                st.markdown(
                    f"**Confirmation #:** {result.get('data', {}).get('id', 'N/A')}")
                st.markdown(f"**Waste Type:** {waste_type}")
                st.markdown(f"**Location:** {pickup_location}")
                st.markdown(f"**Date & Time:** {formatted_datetime}")
                st.markdown(f"**Status:** Pending")

                # Add instructions for tracking
                st.info(
                    "You can track your pickup status using the Track Pickup page.")
            else:
                st.error(f"Failed to schedule pickup: {result.get('error', 'Unknown error')}")

    # Display information about waste types
    st.markdown("<h2 class='sub-header'>Waste Type Information</h2>",
                unsafe_allow_html=True)

    waste_info_tab1, waste_info_tab2, waste_info_tab3, waste_info_tab4 = st.tabs([
        "♻️ Plastic", "🖥️ Electronic", "☢️ Hazardous", "🌱 Organic"
    ])

    with waste_info_tab1:
        st.markdown("""
        ### Plastic Waste Guidelines
        
        **Acceptable Items:**
        - Plastic bottles and containers (types 1-7)
        - Plastic bags and wraps
        - Polystyrene foam containers
        
        **Preparation:**
        - Rinse all containers
        - Remove caps and lids
        - Flatten items to save space
        
        **Not Accepted:**
        - Plastic with food residue
        - Plastic medical waste
        - Certain plastic toys with electronics
        """)

    with waste_info_tab2:
        st.markdown("""
        ### Electronic Waste Guidelines
        
        **Acceptable Items:**
        - Computers and laptops
        - Mobile phones and tablets
        - Printers and scanners
        - Electronic appliances
        - Cables and chargers
        
        **Preparation:**
        - Remove batteries if possible
        - Clear personal data from devices
        - Package items securely
        
        **Not Accepted:**
        - Items with leaking batteries
        - Smoke detectors
        - Large appliances (schedule special pickup)
        """)

    with waste_info_tab3:
        st.markdown("""
        ### Hazardous Waste Guidelines
        
        **Acceptable Items:**
        - Batteries
        - Paint and solvents
        - Cleaning chemicals
        - Motor oil and antifreeze
        - Pesticides and herbicides
        - Fluorescent bulbs
        
        **Preparation:**
        - Keep in original containers when possible
        - Secure lids and prevent leaks
        - Label unidentified materials if possible
        
        **Not Accepted:**
        - Explosive materials
        - Radioactive waste
        - Medical waste (contact healthcare provider)
        """)

    with waste_info_tab4:
        st.markdown("""
        ### Organic Waste Guidelines
        
        **Acceptable Items:**
        - Food scraps
        - Yard trimmings
        - Paper towels and napkins
        - Coffee grounds and filters
        - Tea bags
        
        **Preparation:**
        - Remove packaging
        - Keep separate from other waste types
        - Use compostable bags if available
        
        **Not Accepted:**
        - Plastic or biodegradable plastic bags
        - Pet waste
        - Large tree branches
        - Diseased plants
        """)
