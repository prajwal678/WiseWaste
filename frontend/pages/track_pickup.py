import streamlit as st
import pandas as pd
from utils.api import get_pickup_by_id, get_all_pickups, update_pickup, delete_pickup
from utils.formatting import format_datetime, display_pickup_card


def show_track_pickup():
    st.markdown("<h1 class='main-header'>Track Waste Pickup</h1>",
                unsafe_allow_html=True)

    # Create tabs for different tracking methods
    tab1, tab2 = st.tabs(["Track by ID", "My Pickups"])

    with tab1:
        # Track by ID
        st.markdown("<h2 class='sub-header'>Find by Pickup ID</h2>",
                    unsafe_allow_html=True)

        col1, col2 = st.columns([3, 1])
        with col1:
            pickup_id = st.number_input("Enter Pickup ID", min_value=1, step=1)
        with col2:
            track_button = st.button("Track", use_container_width=True)

        if track_button and pickup_id:
            # Get pickup details from API
            pickup_data = get_pickup_by_id(pickup_id)
            if isinstance(pickup_data, dict) and "error" in pickup_data:
                st.error(f"Error: {pickup_data['error']}")
            elif pickup_data:
                st.success(f"Pickup #{pickup_id} found!")
                display_pickup_details(pickup_data)
            else:
                st.error(f"No pickup found with ID {pickup_id}")

    with tab2:
        # Show all pickups for current user
        st.markdown("<h2 class='sub-header'>My Pickup Requests</h2>",
                    unsafe_allow_html=True)
        if st.button("Refresh My Pickups"):
            st.experimental_rerun()

        # Get all pickups from the API and filter by the logged-in user
        all_pickups = get_all_pickups()
        if isinstance(all_pickups, dict) and "error" in all_pickups:
            st.error(f"Error: {all_pickups['error']}")
        else:
            user_pickups = [p for p in all_pickups if p.get(
                "userName") == st.session_state.user_name]
            if not user_pickups:
                st.info("You don't have any pickup requests yet.")
                if st.button("Request a Pickup"):
                    st.session_state.page = 'request_pickup'
                    st.experimental_rerun()
            else:
                # Sort by creation timestamp (most recent first)
                user_pickups.sort(key=lambda p: p.get(
                    "creationTimestamp", ""), reverse=True)

                # Group pickups by status
                pending_pickups = [
                    p for p in user_pickups if p.get("status") == "Pending"]
                scheduled_pickups = [
                    p for p in user_pickups if p.get("status") == "Scheduled"]
                completed_pickups = [
                    p for p in user_pickups if p.get("status") == "Completed"]

                # Display grouped pickups in expanders
                with st.expander("Pending Pickups", expanded=len(pending_pickups) > 0):
                    if pending_pickups:
                        for pickup in pending_pickups:
                            col1, col2 = st.columns([3, 1])
                            with col1:
                                display_pickup_card(pickup)
                            with col2:
                                st.markdown("<br><br>", unsafe_allow_html=True)
                                if st.button("View Details", key=f"view_{pickup['id']}"):
                                    st.session_state.selected_pickup = pickup
                                    st.experimental_rerun()
                    else:
                        st.info("No pending pickups.")

                with st.expander("Scheduled Pickups", expanded=len(scheduled_pickups) > 0):
                    if scheduled_pickups:
                        for pickup in scheduled_pickups:
                            col1, col2 = st.columns([3, 1])
                            with col1:
                                display_pickup_card(pickup)
                            with col2:
                                st.markdown("<br><br>", unsafe_allow_html=True)
                                if st.button("View Details", key=f"view_{pickup['id']}"):
                                    st.session_state.selected_pickup = pickup
                                    st.experimental_rerun()
                    else:
                        st.info("No scheduled pickups.")

                with st.expander("Completed Pickups", expanded=False):
                    if completed_pickups:
                        for pickup in completed_pickups:
                            col1, col2 = st.columns([3, 1])
                            with col1:
                                display_pickup_card(pickup)
                            with col2:
                                st.markdown("<br><br>", unsafe_allow_html=True)
                                if st.button("View Details", key=f"view_{pickup['id']}"):
                                    st.session_state.selected_pickup = pickup
                                    st.experimental_rerun()
                    else:
                        st.info("No completed pickups.")

    # If a pickup is selected, display detailed information along with update/delete options
    if 'selected_pickup' in st.session_state and st.session_state.selected_pickup:
        st.markdown("<hr>", unsafe_allow_html=True)
        st.markdown("<h2 class='sub-header'>Pickup Details</h2>",
                    unsafe_allow_html=True)
        display_pickup_details(st.session_state.selected_pickup)

        if st.button("Close Details"):
            del st.session_state.selected_pickup
            st.experimental_rerun()


def display_pickup_details(pickup):
    """Display detailed information about a pickup with update/delete options."""
    col1, col2 = st.columns([2, 1])
    with col1:
        st.markdown(f"#### {pickup.get('wasteType', 'Unknown')
                            } Waste Pickup #{pickup.get('id', 'N/A')}")
        st.markdown(f"**Status:** {pickup.get('status', 'Unknown')}")
        st.markdown(f"**Location:** {pickup.get('pickupLocation', 'Unknown')}")
        st.markdown(
            f"**Scheduled for:** {format_datetime(pickup.get('pickupDateTime', ''))}")
        st.markdown(
            f"**Created At:** {format_datetime(pickup.get('creationTimestamp', ''))}")
    with col2:
        if pickup.get('status') in ["Pending", "Scheduled"]:
            if st.button("Update Pickup", key=f"update_{pickup.get('id', '')}"):
                update_pickup_form(pickup)
            if st.button("Cancel Pickup", key=f"cancel_{pickup.get('id', '')}"):
                result = delete_pickup(pickup.get('id', ''))
                if result.get("success", False):
                    st.success("Pickup cancelled successfully!")
                    st.experimental_rerun()
                else:
                    st.error(f"Failed to cancel pickup: {
                             result.get('error', 'Unknown error')}")


def update_pickup_form(pickup):
    """Display a form to update pickup details."""
    st.markdown("### Update Pickup Details")
    with st.form(f"update_form_{pickup.get('id', '')}"):
        new_waste_type = st.selectbox(
            "Waste Type",
            options=["Plastic", "Electronic", "Hazardous", "Organic"],
            index=["Plastic", "Electronic", "Hazardous", "Organic"].index(
                pickup.get("wasteType", "Plastic"))
        )
        new_pickup_location = st.text_input(
            "Pickup Location", value=pickup.get("pickupLocation", ""))
        new_pickup_datetime = st.text_input(
            "Pickup DateTime (YYYY-MM-DD HH:MM:SS)", value=pickup.get("pickupDateTime", ""))
        new_status = st.selectbox(
            "Status",
            options=["Pending", "Scheduled", "Completed"],
            index=["Pending", "Scheduled", "Completed"].index(
                pickup.get("status", "Pending"))
        )
        new_user_name = st.text_input(
            "User Name", value=pickup.get("userName", ""))
        submitted = st.form_submit_button("Update Pickup")

        if submitted:
            result = update_pickup(
                pickup.get("id", ""),
                waste_type=new_waste_type,
                pickup_location=new_pickup_location,
                pickup_date_time=new_pickup_datetime,
                status=new_status,
                user_name=new_user_name
            )
            if result.get("success", False):
                st.success("Pickup updated successfully!")
                if 'selected_pickup' in st.session_state:
                    del st.session_state.selected_pickup
                st.experimental_rerun()
            else:
                st.error(f"Failed to update pickup: {
                         result.get('error', 'Unknown error')}")
