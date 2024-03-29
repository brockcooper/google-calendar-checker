import datetime
import pytz
import os.path
import json
import boto3
from googleapiclient.discovery import build
from googleapiclient.errors import HttpError
from google.oauth2 import service_account

SCOPES = ['https://www.googleapis.com/auth/calendar.readonly']
BUCKET_NAME = os.environ.get("BUCKET")
FILE_NAME = os.environ.get("FILE")
s3 = boto3.client('s3')

def get_s3_json_content(bucket, key):
    object_content = s3.get_object(Bucket=bucket, Key=key)['Body'].read()
    return json.loads(object_content)


def handler(event, context):
    json_content = get_s3_json_content(BUCKET_NAME, FILE_NAME)
    creds = service_account.Credentials.from_service_account_info(
        json_content, scopes=SCOPES)
    try:
        service = build('calendar', 'v3', credentials=creds)

        # Call the Calendar API
        now = datetime.datetime.utcnow().isoformat() + 'Z'  # 'Z' indicates UTC time
        events_result = service.events().list(calendarId=os.environ.get("EMAIL"), timeMin=now,
                                              maxResults=15, singleEvents=True,
                                              orderBy='startTime').execute()
        events = events_result.get('items', [])

        events_list = []
        busy = False
        current_time = datetime.datetime.now(pytz.timezone('US/Mountain'))
        work_hours = True

        # Returns the start and name of the next 15 events
        for event in events:
            start = event['start'].get('dateTime', event['start'].get('date'))
            # Convert start to a timestamp instead of a full date
            if len(start) == 10:
                start = start + 'T00:00:00-07:00'
            start_time = datetime.datetime.strptime(start, "%Y-%m-%dT%H:%M:%S%z")

            end = event['end'].get('dateTime', event['end'].get('date'))
            if len(end) == 10:
                end = end + 'T00:00:00-07:00'
            end_time = datetime.datetime.strptime(end, "%Y-%m-%dT%H:%M:%S%z")

            # Check to see if you're currently busy
            if start_time < current_time < end_time:
                busy = True

            # Tell Arduino to stop checking at night (between 6pm and 8am)
            # if current_time.hour > 17 or current_time.hour < 6:
            #     work_hours = False

            # Don't include all-day events
            if not(start_time.hour == end_time.hour and start_time.minute == end_time.minute):
                event_json = {"start": start, "end": end}
                events_list.append(event_json)

        events_json = {"events": events_list, "busy": busy, "work_hours": work_hours, "message": "success"}
        statusCode = 200

    except HttpError as error:
        events_json = {"events": [], "busy": False, "message": "fail", "error": 'An error occurred: %s' % error}
        statusCode = 400

    response = {
        'statusCode': statusCode,
        'body': json.dumps(events_json),
        'headers': {
            'Content-Type': 'application/json',
        },
        'isBase64Encoded': False
    }

    return response

if __name__ == '__main__':
    event = {}
    context = {}
    print(handler(event, context))