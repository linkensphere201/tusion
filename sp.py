import json
json_file = 'sample.json'
with open(json_file) as json_data:
    data = json.load(json_data)
    print(data)

