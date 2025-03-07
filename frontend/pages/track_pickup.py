import streamlit as st
from utils.api import get_pickup_by_id, update_pickup

def show_track_pickup():
    st.markdown("<h1 class='main-header'>Track Waste Pickup</h1>",
                unsafe_allow_html=True)

    # Pickup ID input
    pickup_id = st.text_input(
        "Enter Pickup ID",
        help="Enter the confirmation number from your pickup request"
    )

    if pickup_id:
        # Fetch pickup details
        result = get_pickup_by_id(pickup_id)

        if "error" in result:
            st.error(f"Failed to fetch pickup details: {result['error']}")
        else:
            pickup = result
            st.success("Pickup details found!")

            # Display pickup information
            col1, col2 = st.columns(2)

            with col1:
                st.markdown("### Pickup Details")
                st.markdown(f"**Waste Type:** {pickup['waste_type']}")
                st.markdown(f"**Location:** {pickup['pickup_location']}")
                st.markdown(f"**Date & Time:** {pickup['pickup_date_time']}")
                st.markdown(f"**Status:** {pickup['status']}")

            with col2:
                st.markdown("### Status Timeline")
                statuses = ["pending", "confirmed", "in_progress", "completed", "cancelled"]
                current_status_index = statuses.index(pickup['status'])
                
                for i, status in enumerate(statuses):
                    status_emoji = "⏳" if status == "pending" else \
                                 "✅" if status == "confirmed" else \
                                 "🚛" if status == "in_progress" else \
                                 "🎉" if status == "completed" else "❌"
                    
                    status_color = "#2e7d32" if i <= current_status_index else "#9e9e9e"
                    st.markdown(f"""
                    <div style='color: {status_color};'>
                        {status_emoji} {status.replace('_', ' ').title()}
                    </div>
                    """, unsafe_allow_html=True)

            # Status update section (only for admin)
            if st.session_state.user_name == "admin":
                st.markdown("### Update Status")
                new_status = st.selectbox(
                    "New Status",
                    options=statuses,
                    index=current_status_index
                )

                if new_status != pickup['status']:
                    if st.button("Update Status"):
                        update_result = update_pickup(
                            pickup_id=pickup_id,
                            waste_type=pickup['waste_type'],
                            pickup_location=pickup['pickup_location'],
                            pickup_date_time=pickup['pickup_date_time'],
                            status=new_status,
                            user_name=st.session_state.user_name
                        )

                        if update_result.get("success", False):
                            st.success("Status updated successfully!")
                            st.experimental_rerun()
                        else:
                            st.error(f"Failed to update status: {update_result.get('error', 'Unknown error')}")

            # Add helpful information based on status
            if pickup['status'] == "pending":
                st.info("Your pickup request is being reviewed. Please check back later for updates.")
            elif pickup['status'] == "confirmed":
                st.info("Your pickup has been confirmed! A waste collection team will arrive at the scheduled time.")
            elif pickup['status'] == "in_progress":
                st.info("The waste collection team is currently on their way to your location.")
            elif pickup['status'] == "completed":
                st.success("Your waste has been successfully collected and disposed of!")
            elif pickup['status'] == "cancelled":
                st.error("This pickup request has been cancelled. Please contact support for more information.")

    else:
        st.info("Please enter your pickup ID to track your waste pickup status.")
