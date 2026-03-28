#!/usr/bin/python3
import os  #to import env variables
import cgi
import csv
import sys

# get the data from POST (accept/deny button)
post_data = cgi.FieldStorage();
target_id = post_data.getvalue("id");
new_status = post_data.getvalue("status");

rows = [];
csv_path = "../database/leave_requests.csv";

# read the csv file, copy it and modify the line in the copy
with open(csv_path, mode="r") as f:
    reader = csv.reader(f);
    # loop through the rows
    for row in reader:
        if row[4] == target_id:
            row[5] = new_status;
        rows.append(row);

# write the rows back into the og csv file
with open(csv_path, mode='w', newline='') as f:
    writer = csv.writer(f);
    writer.writerows(rows);

print("Content-Type: text/plain\r\n\r\n");
print("Success");