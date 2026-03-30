import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
import urllib.request
import os
from datetime import datetime
from io import StringIO

# === Завантаження даних ===
def download_vhi_data(save_dir="vhi_data"):
    os.makedirs(save_dir, exist_ok=True)
    for province_id in range(1, 28):
        existing_files = [f for f in os.listdir(save_dir) if f"province_{province_id}_" in f]
        if existing_files:
            continue
        url = f"https://www.star.nesdis.noaa.gov/smcd/emb/vci/VH/get_TS_admin.php?country=UKR&provinceID={province_id}&year1=1981&year2=2024&type=Mean"
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"province_{province_id}_{timestamp}.csv"
        filepath = os.path.join(save_dir, filename)
        try:
            urllib.request.urlretrieve(url, filepath)
        except Exception as e:
            st.warning(f"Помилка завантаження області {province_id}: {e}")

def read_vhi_data(save_dir="vhi_data"):
    frames = []
    for filename in sorted(os.listdir(save_dir)):
        filepath = os.path.join(save_dir, filename)
        province_id = int(filename.split("_")[1])
        with open(filepath, 'r') as f:
            lines = f.readlines()
        clean_lines = []
        for line in lines[2:]:
            line = line.strip()
            line = line.replace('<tt><pre>', '').replace('</pre></tt>', '')
            if line and not line.startswith('<'):
                line = line.rstrip(',')
                clean_lines.append(line)
        data = '\n'.join(clean_lines)
        df = pd.read_csv(StringIO(data), header=None,
                        names=['year','week','SMN','SMT','VCI','TCI','VHI'],
                        skipinitialspace=True)
        df['province_id'] = province_id
        frames.append(df)
    return pd.concat(frames, ignore_index=True)

def clean_df(df):
    df = df.drop(columns=['SMN', 'SMT'], errors='ignore')
    df['year'] = pd.to_numeric(df['year'], errors='coerce').astype('Int64')
    df['week'] = pd.to_numeric(df['week'], errors='coerce').astype('Int64')
    df['VCI'] = pd.to_numeric(df['VCI'], errors='coerce')
    df['TCI'] = pd.to_numeric(df['TCI'], errors='coerce')
    df['VHI'] = pd.to_numeric(df['VHI'], errors='coerce')
    df = df[df['VHI'] != -1]
    df = df.dropna(subset=['VHI'])
    return df

province_mapping = {
    1: ('Черкаська', 24), 2: ('Чернігівська', 25), 3: ('Чернівецька', 26),
    4: ('Кримська', 27), 5: ('Дніпропетровська', 3), 6: ('Донецька', 4),
    7: ('Івано-Франківська', 8), 8: ('Харківська', 20), 9: ('Херсонська', 21),
    10: ('Хмельницька', 22), 11: ('Київська', 10), 12: ('Кіровоградська', 9),
    13: ('Луганська', 11), 14: ('Львівська', 12), 15: ('Миколаївська', 13),
    16: ('Одеська', 14), 17: ('Полтавська', 15), 18: ('Рівненська', 16),
    19: ('Сумська', 17), 20: ('Тернопільська', 18), 21: ('Закарпатська', 7),
    22: ('Вінницька', 1), 23: ('Волинська', 2), 24: ('Запорізька', 5),
    25: ('Житомирська', 6), 26: ('Київська міська', 19), 27: ('Севастопільська', 23),
}

# === Кешуємо дані ===
@st.cache_data
def load_data():
    download_vhi_data()
    df = read_vhi_data()
    df = clean_df(df)
    df['province_name'] = df['province_id'].map(lambda x: province_mapping[x][0])
    df['province_ua_id'] = df['province_id'].map(lambda x: province_mapping[x][1])
    return df

df = load_data()

province_names = sorted(df['province_name'].unique())
year_min = int(df['year'].min())
year_max = int(df['year'].max())

# === Інтерфейс ===
st.title('Аналіз VHI даних по областях України')

# === Дефолтні значення ===
DEFAULT_SERIES = 'VHI'
DEFAULT_PROVINCE = sorted(df['province_name'].unique())[0]
DEFAULT_WEEK = (1, 52)
DEFAULT_YEAR = (year_min, year_max)

# Ініціалізація session_state
if 'series' not in st.session_state:
    st.session_state['series'] = DEFAULT_SERIES
if 'province' not in st.session_state:
    st.session_state['province'] = DEFAULT_PROVINCE
if 'week_range' not in st.session_state:
    st.session_state['week_range'] = DEFAULT_WEEK
if 'year_range' not in st.session_state:
    st.session_state['year_range'] = DEFAULT_YEAR
if 'sort_asc' not in st.session_state:
    st.session_state['sort_asc'] = False
if 'sort_desc' not in st.session_state:
    st.session_state['sort_desc'] = False

# === Інтерфейс ===
st.title('Аналіз VHI даних по областях України')
# Layout: ліва колонка — елементи, права — графіки
col_controls, col_content = st.columns([1, 3])

with col_controls:
    st.subheader('Фільтри')

    if st.button('Reset'):
        st.session_state['series'] = DEFAULT_SERIES
        st.session_state['province'] = DEFAULT_PROVINCE
        st.session_state['week_range'] = DEFAULT_WEEK
        st.session_state['year_range'] = DEFAULT_YEAR
        st.session_state['sort_asc'] = False
        st.session_state['sort_desc'] = False
        st.rerun()

    series = st.selectbox('Часовий ряд', ['VHI', 'VCI', 'TCI'], key='series')
    province = st.selectbox('Область', province_names, key='province')
    week_range = st.slider('Інтервал тижнів', 1, 52, key='week_range')
    year_range = st.slider('Інтервал років', year_min, year_max, key='year_range')
    sort_asc = st.checkbox('Сортувати за зростанням', key='sort_asc')
    sort_desc = st.checkbox('Сортувати за спаданням', key='sort_desc')

# === Фільтрація ===
filtered = df[
    (df['province_name'] == province) &
    (df['week'] >= week_range[0]) & (df['week'] <= week_range[1]) &
    (df['year'] >= year_range[0]) & (df['year'] <= year_range[1])
][['year', 'week', series]].copy()

# Сортування — якщо обидва увімкнені, ігноруємо
if sort_asc and not sort_desc:
    filtered = filtered.sort_values(series, ascending=True)
elif sort_desc and not sort_asc:
    filtered = filtered.sort_values(series, ascending=False)
elif sort_asc and sort_desc:
    st.warning('Увімкнено обидва сортування — сортування ігнорується')

with col_content:
    tab1, tab2, tab3 = st.tabs(['Таблиця', 'Графік', 'Порівняння областей'])

    with tab1:
        st.dataframe(filtered, use_container_width=True)

    with tab2:
        fig, ax = plt.subplots(figsize=(10, 4))
        ax.plot(filtered['week'].astype(str) + '-' + filtered['year'].astype(str),
                filtered[series], color='steelblue')
        ax.set_title(f'{series} для {province}')
        ax.set_xlabel('Тиждень-Рік')
        ax.set_ylabel(series)
        plt.xticks(rotation=45, ha='right', fontsize=6)
        ax.grid(True)
        st.pyplot(fig)

    with tab3:
        # Порівняння обраної області з усіма іншими
        compare = df[
            (df['week'] >= week_range[0]) & (df['week'] <= week_range[1]) &
            (df['year'] >= year_range[0]) & (df['year'] <= year_range[1])
        ].groupby('province_name')[series].mean().sort_values()

        fig2, ax2 = plt.subplots(figsize=(10, 6))
        colors = ['red' if p == province else 'steelblue' for p in compare.index]
        compare.plot(kind='barh', ax=ax2, color=colors)
        ax2.set_title(f'Середнє {series} по всіх областях')
        ax2.set_xlabel(f'Середнє {series}')
        ax2.grid(True, axis='x')
        st.pyplot(fig2)