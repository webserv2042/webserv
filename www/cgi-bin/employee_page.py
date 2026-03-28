#!/usr/bin/python3
import os  #to import env variables
import cgi
import csv

# open the csv database
with open("../database/leave_requests.csv", mode="r") as db_file:
	reader = csv.reader(db_file);

	all_rows = list(reader);

	# loop through the rows in reverse, to get the most recent one first
	requests_html = "";
	for row in reversed(all_rows):
		if len(row) < 6: 
			continue;
		motive, start_date, end_date, number, status = row[1:];
		status_class = "";
		proof_file = "../database/uploaded/request_" + number + ".pdf"
		if status == "pending" :
			status_class = "status-pending";
		if status == "accepted" :
			status_class = "status-accepted";
		if status == "denied" :
			status_class = "status-denied";

		requests_html += f"""
		<div class="request">
            <p class="request-motive">{motive}</p>
            <p class="request-dates">{start_date} - {end_date}</p>
            <p class="request-status {status_class}">{status}</p>
        </div>
        <hr class="request-divider">
		""";

#open the template file safely, closing it automatically after use, and store in in a variable
with open("employee_page_template.html", "r") as f:
	template = f.read();

final_output = template.replace("{{REQUESTS}}", requests_html);

print("Content-Type: text/html\r\n\r\n");
print(final_output);

