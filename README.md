# Lambda Function to return Google Calendar events

### Purpose

This repo will deploy a lambda function which can be called as an API which will grab credentials from S3, grab your recent events, and return them as a JSON object. This is intended for an Arduino to make the REST call, then turn on an LED light if I'm currently in a meeting.

### How to Deploy

1. Set up Template in your own Github account

Click the "Use this Template" button in Github and create a new repo using this template

2. Set up the Google Calendar API with a Service Account

Follow the Python Quickstart to enable it. Use a service account and share your Google calendar with that service account email address.

3. Set up Github Repo Settings

In your Github repo, go to settings, Secrets, and add 4 secrets with their respective credentials. You will want to use a user that has the appropriate role to deploy a Lambda function and API Gateway using a CloudFormation template.:

* `AWS_ACCESS_KEY_ID`
* `AWS_SECRET_ACCESS_KEY`
* `AWS_DEFAULT_REGION`
* `BUCKET_NAME`: The S3 bucket name where your credentials.json file exists
* `FILE_NAME`: The file name of your credentials file, should be credentials.json
* `EMAIL`: The email address of the calendar you want to check

You should also protect your `main` and `dev` branches to not allow for deletion, directly pushing to those branches without a Pull Request, and to require the right permissions to only certain users to approve Pull Requests.

5. Push API Gateway and Lambda function to AWS

This repo has a built in CI/CD pipeline using Github Actions found in `.github/workflows/deploy.yml`. All developement branches should be built off the `dev` branch. Once the branch is pushed to the Github repo, a small amount of tests will run to ensure the code is working as intended. When the development branch gets merged into the `dev` branch, Github will deploy a dev version of the API Gateway and Lambda to AWS, with a naming convention of `dev` added to the name. Finally, when `dev` is merged into `main` the true production version of the application will be available for use.

The branches should look similar to this flow:

```
           < feature_branch_1
           < feature_branch_2
main < dev < feature_branch_3
           < feature_branch_4
           < feature_branch_5
```

### Explanation of Files
* `.github/workflows/deploy.yml`: Uses Github Actions for the CI/CD pipeline
* `requirements.txt`: Lists the required packages needed to run the code locally and deploy to Lambda. You can run `pip install -r requirements.txt` on your local environment to download the required packages for your local development use
* `serverless.yml`: Uses the serverless framework to deploy API Gateway and Lambda to AWS. See `.github/workflows/deploy.yml` for the code that runs to deploy with serverless.
* `lambda_function.py`: This is the actual Python code that will run on the Lambda function
* `google-cal-checker-arduino`: This the Arduino code, meant for an Arduino Uno WiFi Rev 2, that will check your API and turn on an LED light to indicate if you are in a meeting or not.

## Personal Note

I made this project for a requirement for the Hacker badge in the Strenous Life Program. This project ended up being quite a big stretch having never worked with a Google API, figuring out how to integrate my work calendar to my personal program, working with Google Cloud and service accounts with a Gmail account, and having never touched an Arduino or other hardware-based project. It ended up being an huge learning experience integrating all the various pieces together to make this work, even though the code isn't too complicated. I ended up connecting the Arduino to an "On Air" neon sign with a relay module which controls turing the sign on and off, so now my kids can be aware of when I'm on a meeting before barging into my home office.