import os
import tkinter as tk
from tkinter import filedialog, ttk
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

class Plotter:
    def __init__(self, master):
        self.master = master
        self.master.title("CSV Plotter")

        # create a listbox to display the CSV files in the selected folder
        self.file_listbox = tk.Listbox(master, height=50, width=50)
        self.file_listbox.grid(row=0, column=0, columnspan=2, padx=10, pady=10)

        # create a button to select the folder containing the CSV files
        self.folder_btn = ttk.Button(master, text="Select Folder", command=self.get_folder_path)
        self.folder_btn.grid(row=1, column=0, padx=10, pady=10)

        # create a button to generate the plots
        self.plot_btn = ttk.Button(master, text="Generate Plots", command=self.generate_plots)
        self.plot_btn.grid(row=1, column=1, padx=10, pady=10)

        # create a figure to display the plots
        self.fig = plt.figure(figsize=(10, 9), layout="tight")
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.master)
        self.canvas.get_tk_widget().grid(row=0, column=2, rowspan=3, padx=10, pady=10)

        self.file_path = ""
        self.data = None

    def get_folder_path(self):
        folder_path = filedialog.askdirectory()
        self.file_listbox.delete(0, tk.END)
        for filename in os.listdir(folder_path):
            if (filename.endswith(".txt") | filename.endswith(".csv")):
                self.file_listbox.insert(tk.END, filename)
        self.file_path = folder_path

    def generate_plots(self):
        # clear any previously plotted data
        self.fig.clf()

        # get the selected filename from the listbox
        filename = self.file_listbox.get(tk.ACTIVE)
        filepath = os.path.join(self.file_path, filename)

        # read in the data from the selected CSV file
        self.data = pd.read_csv(filepath)

        # create the first plot
        ax1 = self.fig.add_subplot(3, 1, 1)
        ax1.plot(self.data.iloc[:, 0], self.data.iloc[:, 1])
        ax1.set_xlabel('Time and Date')
        ax1.set_ylabel('Relative Humidity')
        ax1.set_title('Humidity')

        # create the second plot
        ax2 = self.fig.add_subplot(3, 1, 2)
        ax2.plot(self.data.iloc[:, 0], self.data.iloc[:, 2])
        ax2.set_xlabel('Time and Date')
        ax2.set_ylabel('Weight in Pounds')
        ax2.set_title('Hive Weight')

        # create the third plot with two lines
        ax3 = self.fig.add_subplot(3, 1, 3)
        ax3.plot(self.data.iloc[:, 0], self.data.iloc[:, 3], label="Internal")
        ax3.plot(self.data.iloc[:, 0], self.data.iloc[:, 4], label="External")
        ax3.set_xlabel('Time and Date')
        ax3.set_ylabel('Temperature In Degrees Celsius')
        ax3.set_title('Internal and External Temperature')
        ax3.legend()

        # redraw the canvas with the new plots
        self.canvas.draw()

root = tk.Tk()
app = Plotter(root)
root.mainloop()