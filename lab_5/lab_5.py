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
