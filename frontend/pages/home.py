import streamlit as st
import pandas as pd
import altair as alt
from utils.api import get_all_pickups, get_environmental_impact
from datetime import datetime


def show_home():
    st.markdown("<h1 class='main-header'>Smart Waste Management Dashboard</h1>",
                unsafe_allow_html=True)

    # Get environmental impact data
    impact_data = get_environmental_impact()

    # Get all pickups for statistics
    all_pickups = get_all_pickups()

    # Check if API call was successful
    if isinstance(impact_data, dict) and "error" in impact_data:
        st.error(f"Could not load environmental impact data: {
                 impact_data['error']}")
        impact_data = {"totalRecycledWaste": 0, "co2Reduction": 0,
                       "wasteTypeDistribution": {}, "totalCompletedPickups": 0}

    if isinstance(all_pickups, dict) and "error" in all_pickups:
        st.error(f"Could not load pickup data: {all_pickups['error']}")
        all_pickups = []

    # Display environmental impact metrics
    st.markdown("<h2 class='sub-header'>Environmental Impact</h2>",
                unsafe_allow_html=True)

    col1, col2, col3 = st.columns(3)

    with col1:
        st.metric(
            label="Total Waste Recycled",
            value=f"{impact_data.get('totalRecycledWaste', 0):.1f} kg",
            delta=None
        )

    with col2:
        st.metric(
            label="CO₂ Emissions Reduced",
            value=f"{impact_data.get('co2Reduction', 0):.1f} kg",
            delta=None
        )

    with col3:
        st.metric(
            label="Completed Pickups",
            value=impact_data.get('totalCompletedPickups', 0),
            delta=None
        )

    # Create visualizations
    st.markdown("<h2 class='sub-header'>Waste Distribution</h2>",
                unsafe_allow_html=True)

    # Waste Type Distribution
    waste_dist = impact_data.get('wasteTypeDistribution', {})
    if waste_dist:
        waste_df = pd.DataFrame(list(waste_dist.items()), columns=[
                                'Waste Type', 'Count'])

        # Create pie chart
        chart = alt.Chart(waste_df).mark_arc().encode(
            theta=alt.Theta(field="Count", type="quantitative"),
            color=alt.Color(field="Waste Type", type="nominal",
                            scale=alt.Scale(scheme='category10')),
            tooltip=['Waste Type', 'Count']
        ).properties(
            width=400,
            height=300,
            title='Waste Type Distribution'
        )

        st.altair_chart(chart, use_container_width=True)
    else:
        st.info("No waste collection data available yet.")

    # Recent Activity
    st.markdown("<h2 class='sub-header'>Recent Activity</h2>",
                unsafe_allow_html=True)

    if all_pickups and len(all_pickups) > 0:
        # Convert to DataFrame
        df = pd.DataFrame(all_pickups)

        # Sort by creation timestamp (most recent first)
        if 'creationTimestamp' in df.columns:
            df['creationTimestamp'] = pd.to_datetime(df['creationTimestamp'])
            df = df.sort_values('creationTimestamp', ascending=False)

        # Display recent pickups
        recent_pickups = df.head(5)

        for _, pickup in recent_pickups.iterrows():
            status_color = "green" if pickup['status'] == "Completed" else "blue" if pickup['status'] == "Scheduled" else "orange"

            st.markdown(f"""
            <div style="border:1px solid #ddd; padding:10px; margin-bottom:10px; border-left:5px solid {status_color};">
                <p><strong>{pickup['wasteType']} Waste</strong> pickup at {pickup['pickupLocation']}</p>
                <p>Status: <span style="color:{status_color};">{pickup['status']}</span> •
                Scheduled for: {pickup['pickupDateTime']} •
                User: {pickup['userName']}</p>
            </div>
            """, unsafe_allow_html=True)
    else:
        st.info("No recent activity yet.")

    # Quick Tips
    st.markdown("<h2 class='sub-header'>Waste Management Tips</h2>",
                unsafe_allow_html=True)

    tips_col1, tips_col2 = st.columns(2)

    with tips_col1:
        st.markdown("""
        #### Plastic Waste
        - Rinse containers before recycling
        - Remove caps and lids
        - Check for recycling symbols (1-7)
        
        #### Electronic Waste
        - Remove batteries before disposal
        - Wipe personal data from devices
        - Consider donating working electronics
        """)

    with tips_col2:
        st.markdown("""
        #### Hazardous Waste
        - Keep in original containers if possible
        - Never mix hazardous materials
        - Store away from heat sources
        
        #### Organic Waste
        - Consider home composting
        - Remove packaging before disposal
        - Avoid mixing with non-organic waste
        """)
