import requests

# replace the "demo" apikey below with your own key from https://www.alphavantage.co/support/#api-key
url = 'https://www.alphavantage.co/query?function=TIME_SERIES_WEEKLY&symbol=TM&apikey=VD6QVQHNLN8RUJFW&datatype=csv'
r = requests.get(url)

# creating the file
csv_file_path = 'weekly_toyota.csv'

# writing into the file
with open(csv_file_path, 'w') as file:
    file.write(r.text)

# confirming it is done
print(f"CSV file saved as {csv_file_path}")