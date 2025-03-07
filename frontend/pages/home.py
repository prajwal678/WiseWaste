import streamlit as st
from utils.api import get_environmental_impact

def show_home():
    st.markdown("<h1 class='main-header'>Welcome to Smart Waste Management</h1>",
                unsafe_allow_html=True)

    # Display environmental impact information
    st.markdown("<h2 class='sub-header'>Environmental Impact</h2>",
                unsafe_allow_html=True)

    # Get environmental impact data
    impact_data = get_environmental_impact()

    if "error" in impact_data:
        st.error(f"Failed to fetch environmental impact data: {impact_data['error']}")
    else:
        # Display impact metrics
        col1, col2, col3 = st.columns(3)

        with col1:
            st.metric(
                "CO2 Emissions Reduced",
                f"{impact_data.get('co2_reduced', 0):.1f} kg",
                help="Estimated CO2 emissions reduced through proper waste management"
            )

        with col2:
            st.metric(
                "Waste Diverted",
                f"{impact_data.get('waste_diverted', 0):.1f} kg",
                help="Total waste diverted from landfills"
            )

        with col3:
            st.metric(
                "Trees Saved",
                f"{impact_data.get('trees_saved', 0):.1f}",
                help="Equivalent number of trees saved through recycling"
            )

    # Quick actions
    st.markdown("<h2 class='sub-header'>Quick Actions</h2>",
                unsafe_allow_html=True)

    col1, col2, col3 = st.columns(3)

    with col1:
        if st.button("🗑️ Request Pickup", use_container_width=True):
            st.session_state.page = 'request_pickup'
            st.experimental_rerun()

    with col2:
        if st.button("🔍 Track Pickup", use_container_width=True):
            st.session_state.page = 'track_pickup'
            st.experimental_rerun()

    with col3:
        if st.button("📋 View All Pickups", use_container_width=True):
            st.session_state.page = 'view_pickups'
            st.experimental_rerun()

    # Waste management guidelines
    st.markdown("<h2 class='sub-header'>Waste Management Guidelines</h2>",
                unsafe_allow_html=True)

    tab1, tab2, tab3, tab4 = st.tabs([
        "♻️ Plastic", "🖥️ Electronic", "☢️ Hazardous", "🌱 Organic"
    ])

    with tab1:
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

    with tab2:
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

    with tab3:
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

    with tab4:
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

    # Contact information
    st.markdown("<h2 class='sub-header'>Contact Us</h2>",
                unsafe_allow_html=True)

    col1, col2 = st.columns(2)

    with col1:
        st.markdown("""
        **Customer Support:**
        - Phone: (555) 123-4567
        - Email: support@wisewaste.com
        - Hours: Mon-Fri, 9:00 AM - 6:00 PM
        """)

    with col2:
        st.markdown("""
        **Emergency Pickup:**
        - Phone: (555) 987-6543
        - Available: 24/7
        - For urgent waste collection needs
        """)
