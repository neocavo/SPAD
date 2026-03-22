import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons
from scipy.signal import butter, filtfilt

# === Початкові параметри ===
INIT_AMPLITUDE = 1.0
INIT_FREQUENCY = 1.0
INIT_PHASE = 0.0
INIT_NOISE_MEAN = 0.0
INIT_NOISE_COV = 0.1

# Час
t = np.linspace(0, 2 * np.pi, 1000)

# Зберігаємо шум окремо щоб не генерувати наново при зміні гармоніки
noise = np.random.normal(INIT_NOISE_MEAN, np.sqrt(INIT_NOISE_COV), len(t))

def harmonic_with_noise(amplitude, frequency, phase, noise_mean, noise_covariance, show_noise):
    # Чиста гармоніка
    clean = amplitude * np.sin(frequency * t + phase)
    if show_noise:
        return clean + noise
    return clean

def apply_filter(signal):
    # Фільтр Баттерворта низьких частот
    b, a = butter(4, 0.1)
    return filtfilt(b, a, signal)

# === Побудова графіка ===
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 7))
plt.subplots_adjust(left=0.1, bottom=0.5)

# Початковий графік
show_noise = [True]
y = harmonic_with_noise(INIT_AMPLITUDE, INIT_FREQUENCY, INIT_PHASE,
                        INIT_NOISE_MEAN, INIT_NOISE_COV, show_noise[0])
y_filtered = apply_filter(y)

line1, = ax1.plot(t, y, label='Гармоніка з шумом', color='blue')
ax1.set_title('Гармоніка з шумом')
ax1.set_xlabel('Час')
ax1.set_ylabel('Амплітуда')
ax1.legend()
ax1.grid(True)

line2, = ax2.plot(t, y_filtered, label='Відфільтрована гармоніка', color='orange')
ax2.set_title('Відфільтрована гармоніка')
ax2.set_xlabel('Час')
ax2.set_ylabel('Амплітуда')
ax2.legend()
ax2.grid(True)

# === Слайдери ===
ax_amp   = plt.axes([0.1, 0.40, 0.65, 0.03])
ax_freq  = plt.axes([0.1, 0.35, 0.65, 0.03])
ax_phase = plt.axes([0.1, 0.30, 0.65, 0.03])
ax_nmean = plt.axes([0.1, 0.25, 0.65, 0.03])
ax_ncov  = plt.axes([0.1, 0.20, 0.65, 0.03])

s_amp   = Slider(ax_amp,   'Амплітуда',    0.1, 5.0, valinit=INIT_AMPLITUDE)
s_freq  = Slider(ax_freq,  'Частота',      0.1, 5.0, valinit=INIT_FREQUENCY)
s_phase = Slider(ax_phase, 'Фаза',         0.0, 2*np.pi, valinit=INIT_PHASE)
s_nmean = Slider(ax_nmean, 'Шум (середнє)', -1.0, 1.0, valinit=INIT_NOISE_MEAN)
s_ncov  = Slider(ax_ncov,  'Шум (дисперсія)', 0.01, 1.0, valinit=INIT_NOISE_COV)
