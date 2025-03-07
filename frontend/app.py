import streamlit as st
from pages.home import show_home
from pages.request_pickup import show_request_pickup
from pages.track_pickup import show_track_pickup
from pages.view_pickups import show_view_pickups

st.set_page_config(
    page_title="Smart Waste Management System",
    page_icon="♻️",
    layout="wide",
    initial_sidebar_state="expanded",
)

# Custom CSS
st.markdown("""
<style>
    .main-header {
        font-size: 2rem;
        color: #2e7d32;
        text-align: center;
        margin-bottom: 1rem;
    }
    .sub-header {
        font-size: 1.5rem;
        color: #1b5e20;
        margin-bottom: 0.5rem;
    }
    .info-text {
        color: #37474f;
    }
    .success-message {
        color: #2e7d32;
        font-weight: bold;
    }
    .error-message {
        color: #d32f2f;
        font-weight: bold;
    }
</style>
""", unsafe_allow_html=True)

# Initialize session state for navigation
if 'page' not in st.session_state:
    st.session_state.page = 'home'

# Initialize session state for user
if 'user_name' not in st.session_state:
    st.session_state.user_name = ''

# Sidebar for navigation
with st.sidebar:
    st.title("Navigation")

    # User name input
    if not st.session_state.user_name:
        st.session_state.user_name = st.text_input(
            "Enter your name to continue:", key="user_name_input")
    else:
        st.write(f"Welcome, {st.session_state.user_name}!")

        # Show navigation options when user has entered name
        if st.button("🏠 Home"):
            st.session_state.page = 'home'
        if st.button("🗑️ Request Pickup"):
            st.session_state.page = 'request_pickup'
        if st.button("🔍 Track Pickup"):
            st.session_state.page = 'track_pickup'
        if st.button("📋 View All Pickups"):
            st.session_state.page = 'view_pickups'

        # Logout option
        if st.button("🚪 Logout"):
            st.session_state.user_name = ''
            st.session_state.page = 'home'
            st.experimental_rerun()

    st.divider()
    st.caption("Hyperlocal Smart Waste Management System")
    st.caption("© 2025 EcoSmart Solutions")

# Render the selected page
if not st.session_state.user_name:
    # Show welcome message if no user is logged in
    st.markdown("<h1 class='main-header'>Welcome to Smart Waste Management</h1>",
                unsafe_allow_html=True)
    st.write("Please enter your name in the sidebar to get started.")

    # Show some information about the app
    col1, col2 = st.columns(2)

    with col1:
        st.markdown("<h2 class='sub-header'>About Our Service</h2>",
                    unsafe_allow_html=True)
        st.write("""
        Our Hyperlocal Smart Waste Management System helps you responsibly dispose of:
        - Recyclable materials
        - Hazardous waste
        - Electronic waste
        - Organic waste
        
        Schedule pickups at your convenience and track their status in real-time.
        """)

    with col2:
        st.markdown("<h2 class='sub-header'>Environmental Impact</h2>",
                    unsafe_allow_html=True)
        st.write("""
        By using our service, you'll be able to:
        - Reduce landfill waste
        - Lower carbon emissions
        - Track your environmental contributions
        - Support sustainable waste management
        """)
else:
    # Show the corresponding page based on navigation
    if st.session_state.page == 'home':
        show_home()
    elif st.session_state.page == 'request_pickup':
        show_request_pickup()
    elif st.session_state.page == 'track_pickup':
        show_track_pickup()
    elif st.session_state.page == 'view_pickups':
        show_view_pickups()
