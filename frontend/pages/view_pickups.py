import streamlit as st
from utils.api import get_all_pickups

def show_view_pickups():
    st.markdown("<h1 class='main-header'>View All Pickups</h1>",
                unsafe_allow_html=True)

    # Get all pickups
    result = get_all_pickups()

    if "error" in result:
        st.error(f"Failed to fetch pickups: {result['error']}")
    else:
        pickups = result

        # Filter options
        col1, col2, col3 = st.columns(3)

        with col1:
            status_filter = st.selectbox(
                "Filter by Status",
                options=["all", "pending", "confirmed", "in_progress", "completed", "cancelled"],
                format_func=lambda x: x.replace("_", " ").title() if x != "all" else "All"
            )

        with col2:
            waste_type_filter = st.selectbox(
                "Filter by Waste Type",
                options=["all", "plastic", "electronic", "hazardous", "organic"],
                format_func=lambda x: x.title() if x != "all" else "All"
            )

        with col3:
            user_filter = st.text_input(
                "Filter by User",
                placeholder="Enter user name"
            )

        # Apply filters
        filtered_pickups = pickups
        if status_filter != "all":
            filtered_pickups = [p for p in filtered_pickups if p["status"] == status_filter]
        if waste_type_filter != "all":
            filtered_pickups = [p for p in filtered_pickups if p["waste_type"] == waste_type_filter]
        if user_filter:
            filtered_pickups = [p for p in filtered_pickups if user_filter.lower() in p["user_name"].lower()]

        # Display pickups
        if not filtered_pickups:
            st.info("No pickups found matching the selected filters.")
        else:
            # Sort pickups by date (most recent first)
            filtered_pickups.sort(key=lambda x: x["pickup_date_time"], reverse=True)

            # Display each pickup in a card format
            for pickup in filtered_pickups:
                with st.expander(f"Pickup #{pickup['id']} - {pickup['waste_type'].title()} Waste"):
                    col1, col2 = st.columns(2)

                    with col1:
                        st.markdown("### Details")
                        st.markdown(f"**Status:** {pickup['status'].replace('_', ' ').title()}")
                        st.markdown(f"**Location:** {pickup['pickup_location']}")
                        st.markdown(f"**Date & Time:** {pickup['pickup_date_time']}")
                        st.markdown(f"**User:** {pickup['user_name']}")

                    with col2:
                        # Status indicator
                        status_color = {
                            "pending": "#ffa000",
                            "confirmed": "#2196f3",
                            "in_progress": "#4caf50",
                            "completed": "#2e7d32",
                            "cancelled": "#d32f2f"
                        }.get(pickup['status'], "#757575")

                        st.markdown(f"""
                        <div style='background-color: {status_color}; color: white; padding: 10px; border-radius: 5px; text-align: center;'>
                            {pickup['status'].replace('_', ' ').title()}
                        </div>
                        """, unsafe_allow_html=True)

                        # Waste type indicator
                        waste_type_emoji = {
                            "plastic": "♻️",
                            "electronic": "🖥️",
                            "hazardous": "☢️",
                            "organic": "🌱"
                        }.get(pickup['waste_type'], "🗑️")

                        st.markdown(f"""
                        <div style='text-align: center; font-size: 2em;'>
                            {waste_type_emoji}
                        </div>
                        """, unsafe_allow_html=True)

            # Display summary statistics
            st.markdown("### Summary Statistics")
            col1, col2, col3 = st.columns(3)

            with col1:
                total_pickups = len(filtered_pickups)
                st.metric("Total Pickups", total_pickups)

            with col2:
                completed_pickups = len([p for p in filtered_pickups if p["status"] == "completed"])
                st.metric("Completed Pickups", completed_pickups)

            with col3:
                pending_pickups = len([p for p in filtered_pickups if p["status"] in ["pending", "confirmed", "in_progress"]])
                st.metric("Active Pickups", pending_pickups)
