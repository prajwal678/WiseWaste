import streamlit as st
import pandas as pd
from utils.api import get_all_pickups
from utils.formatting import create_pickup_df


def show_view_pickups():
    st.markdown("<h1 class='main-header'>View All Waste Pickups</h1>",
                unsafe_allow_html=True)

    # Add a refresh button to update the pickups list
    if st.button("Refresh Pickups"):
        st.experimental_rerun()

    # Fetch all pickups from the backend
    pickups = get_all_pickups()
    if isinstance(pickups, dict) and "error" in pickups:
        st.error(f"Error: {pickups['error']}")
    elif pickups:
        # Convert the API response to a DataFrame for display
        df = create_pickup_df(pickups)
        st.dataframe(df, use_container_width=True)
    else:
        st.info("No pickup data available.")
