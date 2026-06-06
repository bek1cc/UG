#!/usr/bin/env python3
# =============================================================================
#  UNICATE GAMING - Launcher v2.0
#  Professional SA-MP server launcher with portal UI
#
#  Features:
#  - Modern dark blue portal with Unicate Gaming branding
#  - Server status check (online/offline + player count)
#  - One-click LAUNCH to connect to SA-MP server
#  - Auto-install CEF client plugin to GTA SA folder
#  - News & announcements panel
#  - Settings for GTA SA path
#  - Auto-detect GTA SA installation
#
#  Compile to .exe: pyinstaller --onefile --noconsole --icon=ug.ico launcher.py
# =============================================================================

import tkinter as tk
from tkinter import messagebox, filedialog
import os
import sys
import subprocess
import shutil
import json
import urllib.request
import urllib.error
import threading
import time

# =============================================================================
#  CONFIGURATION - Edit these for your server
# =============================================================================
SERVER_IP = "51.222.12.34"      # Your SA-MP server IP
SERVER_PORT = "7777"             # Your SA-MP server port
SERVER_NAME = "Unicate Gaming"
SERVER_WEBSITE = "https://ug-ogc.com"
LAUNCHER_VERSION = "2.0"

# =============================================================================
#  COLORS - Unicate Gaming Blue Theme
# =============================================================================
COLOR_BG_DARK = "#0a1628"
COLOR_BG_MAIN = "#0f2040"
COLOR_BG_LIGHT = "#152d50"
COLOR_BG_ROW = "#1a3a6a"
COLOR_ACCENT = "#2e86de"
COLOR_BRIGHT = "#54a0ff"
COLOR_CYAN = "#00d2ff"
COLOR_WHITE = "#ffffff"
COLOR_TEXT_LIGHT = "#8cb4e0"
COLOR_TEXT_GRAY = "#5b7fa5"
COLOR_GREEN = "#30d158"
COLOR_RED = "#ff453a"
COLOR_GOLD = "#ffd60a"
COLOR_BTN_BLUE = "#2e86de"
COLOR_BTN_GREEN = "#30d158"
COLOR_INPUT_BG = "#0c1e36"

# =============================================================================
#  PATHS
# =============================================================================
def get_app_dir():
    """Get the directory where launcher.py is located"""
    if getattr(sys, 'frozen', False):
        return os.path.dirname(sys.executable)
    return os.path.dirname(os.path.abspath(__file__))

def get_gta_paths():
    """Try to auto-detect GTA SA installation"""
    possible_paths = [
        r"C:\Program Files\Rockstar Games\GTA San Andreas",
        r"C:\Program Files (x86)\Rockstar Games\GTA San Andreas",
        r"C:\GTA San Andreas",
        r"C:\Games\GTA San Andreas",
        r"D:\GTA San Andreas",
        r"D:\Games\GTA San Andreas",
        r"E:\GTA San Andreas",
        os.path.expanduser("~/GTA San Andreas"),
    ]

    # Check registry
    try:
        import winreg
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,
            r"SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 12120")
        path, _ = winreg.QueryValueEx(key, "InstallLocation")
        if path and os.path.isdir(path):
            possible_paths.insert(0, path)
    except:
        pass

    try:
        import winreg
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,
            r"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 12120")
        path, _ = winreg.QueryValueEx(key, "InstallLocation")
        if path and os.path.isdir(path):
            possible_paths.insert(0, path)
    except:
        pass

    for p in possible_paths:
        gta_exe = os.path.join(p, "gta_sa.exe")
        samp_exe = os.path.join(p, "samp.exe")
        if os.path.isfile(gta_exe) or os.path.isfile(samp_exe):
            return p

    return ""

def load_settings():
    """Load settings from JSON file"""
    settings_file = os.path.join(get_app_dir(), "settings.json")
    defaults = {
        "gta_path": get_gta_paths(),
        "server_ip": SERVER_IP,
        "server_port": SERVER_PORT,
        "cef_installed": False,
        "nickname": ""
    }
    try:
        if os.path.isfile(settings_file):
            with open(settings_file, 'r') as f:
                saved = json.load(f)
                defaults.update(saved)
    except:
        pass
    return defaults

def save_settings(settings):
    """Save settings to JSON file"""
    settings_file = os.path.join(get_app_dir(), "settings.json")
    try:
        with open(settings_file, 'w') as f:
            json.dump(settings, f, indent=2)
    except:
        pass

# =============================================================================
#  CEF CLIENT INSTALLER
# =============================================================================
def install_cef_client(gta_path):
    """Copy CEF client files to GTA SA folder"""
    launcher_dir = get_app_dir()
    cef_source = os.path.join(launcher_dir, "CEF_Client")

    if not os.path.isdir(cef_source):
        return False, "CEF_Client folder not found in launcher directory!"

    cef_dest = os.path.join(gta_path, "CEF")

    try:
        # Remove old CEF folder if exists
        if os.path.isdir(cef_dest):
            shutil.rmtree(cef_dest)

        # Copy entire CEF_Client folder as CEF
        shutil.copytree(cef_source, cef_dest)
        return True, "CEF client installed successfully!"
    except Exception as e:
        return False, f"Error installing CEF: {str(e)}"

def check_cef_installed(gta_path):
    """Check if CEF client is already installed"""
    if not gta_path:
        return False
    cef_folder = os.path.join(gta_path, "CEF")
    return os.path.isdir(cef_folder)

# =============================================================================
#  SERVER STATUS CHECKER
# =============================================================================
def check_server_status(callback):
    """Check if SA-MP server is online (runs in thread)"""
    try:
        url = f"http://api.samp.southcla.ws/v1/server/{SERVER_IP}:{SERVER_PORT}"
        req = urllib.request.Request(url, headers={'User-Agent': 'UG-Launcher/2.0'})
        with urllib.request.urlopen(req, timeout=5) as response:
            data = json.loads(response.read().decode())
            callback(True, data.get('core', {}).get('players', 0),
                     data.get('core', {}).get('maxplayers', 100))
    except:
        try:
            # Fallback: simple socket check
            import socket
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.settimeout(3)
            server_addr = (SERVER_IP, int(SERVER_PORT))
            # SA-MP query
            packet = b'SAMP'
            for c in SERVER_IP.split('.'):
                packet += bytes([int(c)])
            packet += int(SERVER_PORT).to_bytes(2, 'little')
            packet += b'i'
            sock.sendto(packet, server_addr)
            data, _ = sock.recvfrom(1024)
            sock.close()
            if len(data) > 10:
                players = int.from_bytes(data[11:13], 'little')
                maxplayers = int.from_bytes(data[13:15], 'little')
                callback(True, players, maxplayers)
            else:
                callback(True, -1, -1)
        except:
            callback(False, 0, 0)

# =============================================================================
#  LAUNCHER GUI
# =============================================================================
class UnicateLauncher:
    def __init__(self):
        self.settings = load_settings()
        self.root = tk.Tk()
        self.root.title(f"{SERVER_NAME} - Launcher v{LAUNCHER_VERSION}")
        self.root.geometry("900x650")
        self.root.resizable(False, False)
        self.root.configure(bg=COLOR_BG_DARK)

        # Center window on screen
        self.root.update_idletasks()
        x = (self.root.winfo_screenwidth() // 2) - (900 // 2)
        y = (self.root.winfo_screenheight() // 2) - (650 // 2)
        self.root.geometry(f"900x650+{x}+{y}")

        # Try to set icon
        try:
            ico_path = os.path.join(get_app_dir(), "ug.ico")
            if os.path.isfile(ico_path):
                self.root.iconbitmap(ico_path)
        except:
            pass

        self.server_online = False
        self.player_count = 0
        self.max_players = 0

        self.build_ui()

        # Check server status on startup
        self.refresh_server_status()

    def build_ui(self):
        """Build the complete launcher UI"""
        # ========== TOP BAR ==========
        top_bar = tk.Frame(self.root, bg=COLOR_BG_MAIN, height=60)
        top_bar.pack(fill=tk.X, side=tk.TOP)
        top_bar.pack_propagate(False)

        # Logo text
        logo_frame = tk.Frame(top_bar, bg=COLOR_BG_MAIN)
        logo_frame.pack(side=tk.LEFT, padx=20, pady=10)

        tk.Label(logo_frame, text="UNICATE", font=("Segoe UI", 22, "bold"),
                fg=COLOR_BRIGHT, bg=COLOR_BG_MAIN).pack(side=tk.LEFT)
        tk.Label(logo_frame, text=" GAMING", font=("Segoe UI", 22, "bold"),
                fg=COLOR_CYAN, bg=COLOR_BG_MAIN).pack(side=tk.LEFT)

        # Version label
        tk.Label(top_bar, text=f"v{LAUNCHER_VERSION}", font=("Segoe UI", 10),
                fg=COLOR_TEXT_GRAY, bg=COLOR_BG_MAIN).pack(side=tk.RIGHT, padx=20)

        # ========== MAIN CONTENT ==========
        main = tk.Frame(self.root, bg=COLOR_BG_DARK)
        main.pack(fill=tk.BOTH, expand=True, padx=0, pady=0)

        # --- LEFT PANEL: Portal / News ---
        left_panel = tk.Frame(main, bg=COLOR_BG_MAIN, width=550)
        left_panel.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(15, 7), pady=15)
        left_panel.pack_propagate(False)

        # Welcome section
        tk.Label(left_panel, text="DOBRODOSLI NA PORTAL",
                font=("Segoe UI", 16, "bold"), fg=COLOR_BRIGHT,
                bg=COLOR_BG_MAIN).pack(pady=(20, 5), padx=20, anchor=tk.W)

        tk.Label(left_panel, text="Unicate Gaming RPG Server",
                font=("Segoe UI", 11), fg=COLOR_TEXT_LIGHT,
                bg=COLOR_BG_MAIN).pack(pady=(0, 15), padx=20, anchor=tk.W)

        # Separator
        sep = tk.Frame(left_panel, bg=COLOR_ACCENT, height=2)
        sep.pack(fill=tk.X, padx=20, pady=5)

        # News section
        tk.Label(left_panel, text="NOVOSTI & OBAVIJESTI",
                font=("Segoe UI", 12, "bold"), fg=COLOR_GOLD,
                bg=COLOR_BG_MAIN).pack(pady=(15, 10), padx=20, anchor=tk.W)

        news_items = [
            ("CEF TABLET SYSTEM", "Novi tablet UI sa modernim dizajnom, bounty board, porukama i vise! Pokreni /phone u igri.", COLOR_BRIGHT),
            ("INVENTORY SYSTEM", "Inventar sa ikonicama - pregledaj oruzje, predmete i opremu na moderni nacin.", COLOR_GREEN),
            ("LAPTOP SYSTEM", "Realisticki laptop UI za pristup mrezi, Dark Web, banki i drugim aplikacijama.", COLOR_CYAN),
            ("BOUNTY BOARD", "Postavi nagradu na glavu drugog igraca! Koristi /bounty ili tablet.", COLOR_RED),
            ("NOVI POSLOVI", "Dodano 15+ poslova sa platama i zadacima. Prijavi se na posao!", COLOR_GOLD),
        ]

        for title, desc, color in news_items:
            news_frame = tk.Frame(left_panel, bg=COLOR_BG_LIGHT)
            news_frame.pack(fill=tk.X, padx=20, pady=4)

            tk.Label(news_frame, text=f"  {title}", font=("Segoe UI", 10, "bold"),
                    fg=color, bg=COLOR_BG_LIGHT, anchor=tk.W).pack(fill=tk.X, pady=(5, 0))
            tk.Label(news_frame, text=f"  {desc}", font=("Segoe UI", 9),
                    fg=COLOR_TEXT_GRAY, bg=COLOR_BG_LIGHT, anchor=tk.W,
                    wraplength=480).pack(fill=tk.X, pady=(0, 5))

        # ========== RIGHT PANEL: Controls ==========
        right_panel = tk.Frame(main, bg=COLOR_BG_MAIN, width=300)
        right_panel.pack(side=tk.RIGHT, fill=tk.BOTH, padx=(7, 15), pady=15)
        right_panel.pack_propagate(False)

        # Server Status Section
        tk.Label(right_panel, text="STATUS SERVERA",
                font=("Segoe UI", 12, "bold"), fg=COLOR_BRIGHT,
                bg=COLOR_BG_MAIN).pack(pady=(20, 10))

        # Status indicator
        self.status_frame = tk.Frame(right_panel, bg=COLOR_BG_LIGHT, padx=15, pady=10)
        self.status_frame.pack(fill=tk.X, padx=15, pady=5)

        self.status_dot = tk.Canvas(self.status_frame, width=16, height=16,
                                    bg=COLOR_BG_LIGHT, highlightthickness=0)
        self.status_dot.pack(side=tk.LEFT, padx=(0, 10))
        self.status_dot_circle = self.status_dot.create_oval(2, 2, 14, 14, fill=COLOR_RED, outline="")

        self.status_label = tk.Label(self.status_frame, text="Provjera...",
                font=("Segoe UI", 11, "bold"), fg=COLOR_WHITE, bg=COLOR_BG_LIGHT)
        self.status_label.pack(side=tk.LEFT)

        self.players_label = tk.Label(right_panel, text="Igraci: ---/---",
                font=("Segoe UI", 10), fg=COLOR_TEXT_LIGHT, bg=COLOR_BG_MAIN)
        self.players_label.pack(pady=(5, 15))

        # IP Display
        ip_frame = tk.Frame(right_panel, bg=COLOR_BG_LIGHT, padx=10, pady=8)
        ip_frame.pack(fill=tk.X, padx=15, pady=5)
        tk.Label(ip_frame, text="IP:", font=("Segoe UI", 9), fg=COLOR_TEXT_GRAY,
                bg=COLOR_BG_LIGHT).pack(side=tk.LEFT)
        tk.Label(ip_frame, text=f"{SERVER_IP}:{SERVER_PORT}", font=("Segoe UI", 10, "bold"),
                fg=COLOR_BRIGHT, bg=COLOR_BG_LIGHT).pack(side=tk.LEFT, padx=5)

        # Copy IP button
        copy_btn = tk.Button(ip_frame, text="KOPIRAJ", font=("Segoe UI", 8),
                bg=COLOR_BG_ROW, fg=COLOR_TEXT_LIGHT, relief=tk.FLAT,
                cursor="hand2", command=self.copy_ip)
        copy_btn.pack(side=tk.RIGHT)

        # Separator
        sep2 = tk.Frame(right_panel, bg=COLOR_ACCENT, height=2)
        sep2.pack(fill=tk.X, padx=15, pady=15)

        # Nickname input
        tk.Label(right_panel, text="NADIMAK",
                font=("Segoe UI", 10, "bold"), fg=COLOR_TEXT_LIGHT,
                bg=COLOR_BG_MAIN).pack(pady=(5, 5))

        self.nickname_var = tk.StringVar(value=self.settings.get("nickname", ""))
        nick_entry = tk.Entry(right_panel, textvariable=self.nickname_var,
                font=("Segoe UI", 12), bg=COLOR_INPUT_BG, fg=COLOR_WHITE,
                insertbackground=COLOR_WHITE, relief=tk.FLAT, justify=tk.CENTER)
        nick_entry.pack(fill=tk.X, padx=15, pady=5, ipady=6)

        # GTA Path
        tk.Label(right_panel, text="GTA SA FOLDER",
                font=("Segoe UI", 9), fg=COLOR_TEXT_GRAY,
                bg=COLOR_BG_MAIN).pack(pady=(10, 3))

        path_frame = tk.Frame(right_panel, bg=COLOR_BG_MAIN)
        path_frame.pack(fill=tk.X, padx=15)

        self.path_var = tk.StringVar(value=self.settings.get("gta_path", ""))
        path_entry = tk.Entry(path_frame, textvariable=self.path_var,
                font=("Segoe UI", 8), bg=COLOR_INPUT_BG, fg=COLOR_TEXT_LIGHT,
                insertbackground=COLOR_WHITE, relief=tk.FLAT)
        path_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, ipady=4)

        browse_btn = tk.Button(path_frame, text="...", font=("Segoe UI", 8),
                bg=COLOR_BG_ROW, fg=COLOR_TEXT_LIGHT, relief=tk.FLAT,
                cursor="hand2", command=self.browse_gta_path)
        browse_btn.pack(side=tk.RIGHT, padx=(5, 0), ipady=2)

        # CEF Install button
        self.cef_status_text = "CEF: Provjera..."
        self.cef_btn = tk.Button(right_panel, text=self.cef_status_text,
                font=("Segoe UI", 9), bg=COLOR_BG_ROW, fg=COLOR_TEXT_LIGHT,
                relief=tk.FLAT, cursor="hand2", command=self.install_cef)
        self.cef_btn.pack(fill=tk.X, padx=15, pady=(10, 5), ipady=4)
        self.update_cef_status()

        # ========== LAUNCH BUTTON ==========
        launch_frame = tk.Frame(right_panel, bg=COLOR_BG_MAIN)
        launch_frame.pack(fill=tk.X, padx=15, pady=(20, 10))

        self.launch_btn = tk.Button(launch_frame,
                text="LAUNCH",
                font=("Segoe UI", 20, "bold"),
                bg=COLOR_ACCENT, fg=COLOR_WHITE,
                activebackground=COLOR_BRIGHT, activeforeground=COLOR_WHITE,
                relief=tk.FLAT, cursor="hand2",
                command=self.launch_game)
        self.launch_btn.pack(fill=tk.X, ipady=12)

        # Hover effect for launch button
        self.launch_btn.bind("<Enter>", lambda e: self.launch_btn.configure(bg=COLOR_BRIGHT))
        self.launch_btn.bind("<Leave>", lambda e: self.launch_btn.configure(bg=COLOR_ACCENT))

        # Refresh status button
        refresh_btn = tk.Button(right_panel, text="Osvjezi status",
                font=("Segoe UI", 9), bg=COLOR_BG_LIGHT, fg=COLOR_TEXT_LIGHT,
                relief=tk.FLAT, cursor="hand2", command=self.refresh_server_status)
        refresh_btn.pack(pady=5)

        # ========== BOTTOM BAR ==========
        bottom = tk.Frame(self.root, bg=COLOR_BG_MAIN, height=30)
        bottom.pack(fill=tk.X, side=tk.BOTTOM)
        bottom.pack_propagate(False)

        tk.Label(bottom, text=f"{SERVER_NAME} Launcher v{LAUNCHER_VERSION} | {SERVER_WEBSITE}",
                font=("Segoe UI", 8), fg=COLOR_TEXT_GRAY, bg=COLOR_BG_MAIN).pack(pady=5)

    # ========== ACTIONS ==========

    def copy_ip(self):
        """Copy server IP to clipboard"""
        self.root.clipboard_clear()
        self.root.clipboard_append(f"{SERVER_IP}:{SERVER_PORT}")
        messagebox.showinfo("Kopirano", f"IP adresa {SERVER_IP}:{SERVER_PORT} kopirana!")

    def browse_gta_path(self):
        """Browse for GTA SA folder"""
        folder = filedialog.askdirectory(title="Izaberi GTA San Andreas folder")
        if folder:
            self.path_var.set(folder)
            self.settings["gta_path"] = folder
            save_settings(self.settings)
            self.update_cef_status()

    def update_cef_status(self):
        """Update CEF installation status button"""
        gta_path = self.path_var.get()
        if check_cef_installed(gta_path):
            self.cef_btn.configure(text="CEF: Instaliran  Povuci ponovo", bg=COLOR_BG_ROW, fg=COLOR_GREEN)
        else:
            self.cef_btn.configure(text="CEF: Instaliraj u GTA folder", bg=COLOR_BG_ROW, fg=COLOR_GOLD)

    def install_cef(self):
        """Install CEF client files to GTA SA folder"""
        gta_path = self.path_var.get()
        if not gta_path:
            messagebox.showwarning("Upozorenje", "Prvo izaberi GTA San Andreas folder!")
            return

        if not os.path.isdir(gta_path):
            messagebox.showerror("Greska", "Izabrani folder ne postoji!")
            return

        self.cef_btn.configure(text="CEF: Instalacija...", fg=COLOR_GOLD)

        success, msg = install_cef_client(gta_path)
        if success:
            self.cef_btn.configure(text="CEF: Instaliran!", fg=COLOR_GREEN)
            self.settings["cef_installed"] = True
            save_settings(self.settings)
            messagebox.showinfo("Uspjeh", msg)
        else:
            self.cef_btn.configure(text="CEF: Greska!", fg=COLOR_RED)
            messagebox.showerror("Greska", msg)

        self.update_cef_status()

    def refresh_server_status(self):
        """Check server status in background thread"""
        self.status_label.configure(text="Provjera...")
        self.status_dot.itemconfigure(self.status_dot_circle, fill=COLOR_GOLD)

        def do_check():
            check_server_status(self.on_status_checked)

        thread = threading.Thread(target=do_check, daemon=True)
        thread.start()

    def on_status_clicked(self):
        """Manual refresh on click"""
        self.refresh_server_status()

    def on_status_checked(self, online, players, maxplayers):
        """Callback when server status is checked"""
        self.server_online = online
        self.player_count = players
        self.max_players = maxplayers

        def update_ui():
            if online:
                self.status_dot.itemconfigure(self.status_dot_circle, fill=COLOR_GREEN)
                self.status_label.configure(text="ONLINE", fg=COLOR_GREEN)
                if players >= 0:
                    self.players_label.configure(text=f"Igraci: {players}/{maxplayers}")
                else:
                    self.players_label.configure(text="Igraci: N/A")
            else:
                self.status_dot.itemconfigure(self.status_dot_circle, fill=COLOR_RED)
                self.status_label.configure(text="OFFLINE", fg=COLOR_RED)
                self.players_label.configure(text="Server nije dostupan")

        self.root.after(0, update_ui)

    def launch_game(self):
        """Launch SA-MP and connect to server"""
        gta_path = self.path_var.get()
        nickname = self.nickname_var.get().strip()

        # Save settings
        self.settings["gta_path"] = gta_path
        self.settings["nickname"] = nickname
        save_settings(self.settings)

        if not gta_path:
            messagebox.showwarning("Upozorenje",
                "Izaberi GTA San Andreas folder!\n\nKlikni '...' dugme pored putanje.")
            return

        if not os.path.isdir(gta_path):
            messagebox.showerror("Greska", "GTA San Andreas folder ne postoji!\n\nIzaberi ispravan folder.")
            return

        # Check for samp.exe
        samp_exe = os.path.join(gta_path, "samp.exe")
        if not os.path.isfile(samp_exe):
            messagebox.showerror("Greska",
                "samp.exe nije pronadjen u izabranom folderu!\n\n"
                "Provjeri da li je SA-MP klijent instaliran.")
            return

        # Auto-install CEF if not installed
        if not check_cef_installed(gta_path):
            success, msg = install_cef_client(gta_path)
            if success:
                messagebox.showinfo("CEF Instaliran",
                    "CEF klijent plugin je automatski instaliran u tvoj GTA folder!")
            # Don't block if CEF install fails - game can still work

        # Set nickname in SA-MP
        if nickname:
            try:
                # Write nickname to sa-mp config
                samp_cfg = os.path.join(gta_path, "sa-mp.cfg")
                lines = []
                if os.path.isfile(samp_cfg):
                    with open(samp_cfg, 'r') as f:
                        lines = f.readlines()

                found = False
                for i, line in enumerate(lines):
                    if line.lower().startswith("name"):
                        lines[i] = f"name {nickname}\n"
                        found = True
                        break
                if not found:
                    lines.append(f"name {nickname}\n")

                with open(samp_cfg, 'w') as f:
                    f.writelines(lines)
            except:
                pass

        # Launch samp.exe with server connection
        try:
            os.chdir(gta_path)
            subprocess.Popen([samp_exe, f"{SERVER_IP}:{SERVER_PORT}"],
                           cwd=gta_path)
            messagebox.showinfo("Pokrenuto!",
                f"Povezivanje na {SERVER_NAME}...\n\n"
                f"IP: {SERVER_IP}:{SERVER_PORT}\n"
                f"Nadimak: {nickname if nickname else 'Nije postavljen'}\n\n"
                f"Uzivaj u igri!")
        except Exception as e:
            messagebox.showerror("Greska",
                f"Nije moguce pokrenuti samp.exe!\n\n{str(e)}")

    def run(self):
        """Start the launcher"""
        self.root.mainloop()

# =============================================================================
#  MAIN
# =============================================================================
if __name__ == "__main__":
    app = UnicateLauncher()
    app.run()
