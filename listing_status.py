import csv
import requests

# replace the "demo" apikey below with your own key from https://www.alphavantage.co/support/#api-key
url = 'https://www.alphavantage.co/query?function=LISTING_STATUS&apikey=VD6QVQHNLN8RUJFW'
r = requests.get(url)

# creating file
csv_file_path = 'active_listed.csv'

# writing into file
with open(csv_file_path, 'w') as file:
    file.write(r.text)

print(f"CSV file saved as {csv_file_path}")
