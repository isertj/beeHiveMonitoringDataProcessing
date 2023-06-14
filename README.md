# beeHiveMonitoringDataProcessing

Libraries Used:
  - tkinter: GUI Toolkit
  - matplotlib: Plotting library
  - pandas: Used for data manipulation

Other:
  - PyInstaller: Used for making an executable file

Running Script:
  1. Ensure all necessary libraries are installed. Run the following command in the directory that contains main.py:
    - pip install tk pandas matplotlib
  2. With the librarires installed, the script should be able to run with the following command.
    - python main.py
    - If there are still issues, check which version of python and the above libraries is currently installed 
 
Generating Executable:
  1. Install pyinstaller
    - In the same directory as main.py, enter the command 'pip install pyinstaller'
    - This should download all of the necessary assets for pyinstaller
    - You can use 'pyinstaller --version' to check it's status
  2. Make executable
    - Once installation of pyinstaller has been confirmed, run the commnad 'pyinstaller main.py'
    - This will create both a main.spec and a 'dist' directory that contains the executable, 'main.exe', and libraries

Instructions:
  1. When the window appears, there should not be any data shown. First, the user must give a folder where the data is located.
  2. With a folder selected, the list of files should update. Select one and press "Generate Plots". This should update the blank area on the right. Populating it with three line graphs.
  3. To change the data show, select a different file and press "Generate Plots" again. This should replace the plots with the data taken from the new selected file.
