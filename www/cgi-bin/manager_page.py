#!/usr/bin/python3
import os  #to import env variables
import cgi
import csv
from datetime import datetime
from urllib.parse import unquote

def get_total_vacation(csv_path):
	total_days = 0;
	date_format = "%d-%m-%Y"

	with open(csv_path, mode="r") as csv_file:
		reader = csv.reader(csv_file);
		for row in reader:
			# Check motive and status
			if row[1] == "vacation" and row[5] == "accepted":
				start = datetime.strptime(row[2], date_format)
				end = datetime.strptime(row[3], date_format)
				
				# Calculate difference (returns a timedelta object)
				delta = end - start
				# .days gives you the integer; add 1 if the end date is inclusive
				total_days += delta.days + 1
	return total_days


# open the csv database
with open("../database/leave_requests.csv", mode="r") as db_file:
	reader = csv.reader(db_file)

	# loop through the rows
	requests_html = "";
	for row in reader:
		name, motive, start_date, end_date, number, status = row[0:];
		proof_file = "/database/uploaded/request_" + number + ".pdf"

		if status == "pending" :
			requests_html += f"""
			<div class="request-manager" id="{number}">
				<div class="request-info-manager">
					<p class="request-name-manager">{name}</p>
					<p class="request-motive-manager">{motive}</p>
					<p class="request-dates-manager">{start_date} - {end_date}</p>
					<a href="{proof_file}" class="request-proof-manager">Proof</a>
				</div>
				<div class="request-actions-manager">
					<button class="btn-accept" onclick="acceptRequest('{number}', '{proof_file}')">Accept</button>
					<button class="btn-deny"   onclick="denyRequest('{number}', '{proof_file}')">Deny</button>
				</div>
			</div>
			<hr class="request-divider-manager">
			""";
		
	total_days = 35;
	taken_days = get_total_vacation("../database/leave_requests.csv");
	percentage = (taken_days / total_days) * 100;
	if percentage > 100 :
		percentage = 100;
	
	total_vacation = "";
	total_vacation = (f"""
	<div class="vacation-row">
		<div class="profile-avatar-list">
			<img src="../../uploads/alexis.jpg" alt="Profile photo">
		</div>
		<span class="vacation-count">{taken_days}/{total_days}</span>
		<div class="vacation-bar-track">
			<div class="vacation-bar-fill" style="width: {percentage}%;"></div>
		</div>
	</div>
	<hr class="vacation-divider">
	""");

#open the template file safely, closing it automatically after use, and store in in a variable
with open("manager_page_template.html", "r") as f:
	template = f.read();

final_output = template.replace("{{REQUESTS}}", requests_html) \
						.replace("{{VACATION_TIME}}", total_vacation) \
						.replace("{{NAME}}", "Manager");

print("Content-Type: text/html\r\n\r\n");
print(final_output);