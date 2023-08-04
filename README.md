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


