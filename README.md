# Smart Parking Garage System (RTOS)
A Smart Parking Garage Gate System implemented on the Tiva-C TM4C123GH6PM microcontroller running FreeRTOS. Features multitasking state management, obstacle detection, and prioritized security controls.


##  Environment Setup & Installation Guide

Before you can build and flash the code to the Tiva-C Launchpad, you must configure your local environment.

### Step 1: Install Keil Debugger Patch
Because modern Keil versions deprecated the legacy Tiva-C debugger, you must install the patch provided by the TA.

1. Download `MDK_Stellaris_ICDI_AddOn.exe` from the shared course drive.
2. Close Keil. Run the `.exe` and ensure the destination folder matches your Keil installation directory (usually `C:\Keil_v5`).
3. Plug in your Tiva-C board (switch set to "Debug"). Windows should automatically install the USB drivers in the background.

*⚠️ Troubleshooting (If Windows fails to recognize the board):*
*Only if Keil still cannot find the Stellaris ICDI:* Download `stellaris_icdi_drivers.zip` from the drive, extract it, open Windows Device Manager, right-click the unknown "In-Circuit Debug Interface", and manually update the driver using that extracted folder.

### Step 2: Clone the Repository
Open your terminal and clone this repository to your local machine:
```bash
git clone https://github.com/Nour184/Smart-Parking-Garage.git
```
### Step 3: Open and Configure the Keil Project
1. Navigate to the cloned folder and open `Keil env/smart-parking-prj.uvprojx`.
2. **Resolve Missing Packages:** If Keil shows missing FreeRTOS dependencies, click the **Manage Run-Time Environment** button (the green diamond icon), look at the "Validation Output" at the bottom, and click **Resolve**. Click OK.
3. **Configure the Hardware Debugger (CRITICAL):** Keil might default to Simulator mode, which will fail to flash. You must route it to the physical Tiva-C board:
   * Click the **Options for Target** button (the Magic Wand icon 🪄).
   * Go to the **Debug** tab.
   * On the right side of the window, unselect the **Use:** radio button.
   * Select **Stellaris ICDI** from the dropdown menu.
   * *Next*, go to the **Utilities** tab at the top.
   * Check the box for **Use Target Driver for Flash Programming**.
   * Click **OK** to save.