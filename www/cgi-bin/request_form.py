#!/usr/bin/python3
import os  #to import env variables
import cgi
import csv
import time
from datetime import datetime

if os.environ.get("REQUEST_METHOD") == "POST" :
	form = cgi.FieldStorage(); #get form input

	fileToUpload = form.getvalue("file_upload"); #get the file uploaded

	if fileToUpload :
		timestamp = int(time.time()); #create a unique filename each time
		csv_filename = "leave_requests" + ".csv";
		pdf_filename = "request_" + str(timestamp) + ".pdf";

		csv_path = "../database/" + csv_filename;
		pdf_path = "../database/uploaded/" + pdf_filename;

		new_pdf = open(pdf_path, "wb"); #write in pdf file
		new_pdf.write(fileToUpload);
		new_pdf.close();

		new_csv = open(csv_path, "a"); #write in csv files
		writer = csv.writer(new_csv);

		raw_start = form.getvalue("start-date"); #change date format
		start_obj = datetime.strptime(raw_start, "%Y-%m-%d");
		formatted_start =  start_obj.strftime("%d-%m-%Y");
		raw_end = form.getvalue("end-date"); #change date format
		end_obj = datetime.strptime(raw_end, "%Y-%m-%d");
		formatted_end =  end_obj.strftime("%d-%m-%Y");

		writer.writerow(["Test Name", form.getvalue("motive"), formatted_start, formatted_end, str(timestamp), "pending"]);
		new_csv.close();

		# Redirect after POST
		print("Status: 302 Found")
		print("Location: /cgi-bin/employee_page.py")
		print("Content-Type: text/html\r\n\r\n")
		exit()
	
	else :
		print("Status: 302 Found")
		print("Location: /cgi-bin/request_form.py")
		print("Content-Type: text/html\r\n\r\n")
		exit()


print("Content-Type: text/html\r\n\r\n");

#open the file safely, closing it automatically after use, and store in in a variable
with open("form_template.html", "r") as f:
	template = f.read();
print(template);