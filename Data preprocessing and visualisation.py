import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

# ========= Change to your CSV path =========
file_path = r"C:\Users\lenovo\Desktop\SJCS\low-temp discharge 2.csv"
file_name = os.path.basename(file_path)

# ========= Read data =========
df = pd.read_csv(file_path)
df.columns = df.columns.str.strip()  # Remove hidden spaces

# ========= Intelligent voltage selection listing =========
if 'Battery Voltage' in df.columns:
    df.rename(columns={'Battery Voltage': 'Voltage'}, inplace=True)
elif 'Voltage' not in df.columns:
    raise ValueError("未找到 'Voltage' 或 'Battery Voltage' 列")

# ========= Data preprocessing =========
df.interpolate(method='linear', inplace=True)
df.dropna(subset=['Voltage', 'Current', 'Power'], inplace=True)

# Unification of current units（mA → A）
if df['Current'].max() > 10:
    df['Current'] = df['Current'] / 1000

# Unify current direction (take absolute value)
df['Current'] = df['Current'].abs()

# Time processing
df = df[df['Time'].diff().fillna(0) >= 0]
df = df[df['Time'].diff().fillna(0) < 1000]
df['Time'] = (df['Time'] - df['Time'].iloc[0]) / 3600  # Unit: hours

# ========= drawing =========
plt.rcParams.update({'font.size': 12})
fig, axs = plt.subplots(5, 1, figsize=(14, 18), sharex=True)
plt.subplots_adjust(hspace=0.6)
fig.suptitle(file_name, fontsize=16)

# Voltage
axs[0].plot(df['Time'], df['Voltage'], color='blue', label='Voltage')
axs[0].set_title("Voltage vs Time")
axs[0].set_ylabel("Voltage (V)")
axs[0].legend()
axs[0].grid(True)

# Current
axs[1].plot(df['Time'], df['Current'], color='red', label='Current')
axs[1].set_title("Current vs Time")
axs[1].set_ylabel("Current (A)")
axs[1].legend()
axs[1].grid(True)

# Power
axs[2].plot(df['Time'], df['Power'], color='green', label='Power')
axs[2].set_title("Power vs Time")
axs[2].set_ylabel("Power (W)")
axs[2].legend()
axs[2].grid(True)

# Temperature
if 'Temperature' in df.columns:
    axs[3].plot(df['Time'], df['Temperature'], color='orange')
    axs[3].set_title("Temperature vs Time")
    axs[3].set_ylabel("Temperature (°C)")
    axs[3].grid(True)
else:
    axs[3].axis('off')

# Humidity
if 'Humidity' in df.columns:
    axs[4].plot(df['Time'], df['Humidity'], color='purple')
    axs[4].set_title("Humidity vs Time")
    axs[4].set_ylabel("Humidity (%)")
    axs[4].grid(True)
else:
    axs[4].axis('off')

# Timeline settings
xticks = np.arange(0, df['Time'].max(), 3)
axs[-1].set_xticks(xticks)
axs[-1].set_xticklabels([f"{x:.1f}" for x in xticks])
axs[-1].set_xlabel("Time (hours)", labelpad=10)

plt.tight_layout(rect=[0, 0, 1, 0.97])

# ========= Automatically save to desktop/SJCS folder =========
output_dir = os.path.expanduser("~/Desktop/SJCS")
os.makedirs(output_dir, exist_ok=True)

save_path = os.path.join(output_dir, file_name.replace('.csv', '.png'))
plt.savefig(save_path, dpi=300)

print(f"✅ 图像已保存到：{save_path}")
plt.show()
