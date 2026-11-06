# Google Apps Script setup

The cloud project consists of `Code.gs` and `Dashboard.html` in the **same Apps Script project**.

## 1. Prepare Google Drive and Sheets

Use a dedicated Drive folder for robot attendance images and a spreadsheet for attendance records. The IDs previously supplied for this project are prefilled in `Code.gs`:

- Drive folder ID: `1nmCi334d7ZRbGpJfqxMmyZYaGuH6tZVS`
- Spreadsheet ID: `1O23bEciw68S6AJzO9--faOnqfGIHgn-zvgj6DEAJkX0`

Confirm that the Google account deploying the script has access to both resources. The spreadsheet should contain a sheet/tab named `Attendance`; the script creates it if it does not exist.

## 2. Create the Apps Script project

1. Open Google Apps Script and create a project.
2. Replace the contents of `Code.gs` with the repository's `Code.gs`.
3. Add an HTML file named exactly `Dashboard` (Apps Script displays it as `Dashboard.html`).
4. Paste the repository's `Dashboard.html` into that file.
5. In `Code.gs`, replace `CHANGE_THIS_TO_A_LONG_RANDOM_SECRET` with a long random API key. Keep the same key in the ESP32-CAM firmware.
6. Save the project.

`doGet()` calls `HtmlService.createHtmlOutputFromFile('Dashboard')`. The `.html` extension is not included in that function argument.

## 3. Deploy

1. Select **Deploy → New deployment**.
2. Choose **Web app**.
3. Set **Execute as** to your account.
4. Set access to the narrowest option that works for your deployment and testing. The camera must be able to POST to the endpoint; access settings may require sign-in or an organization policy.
5. Deploy and authorize the requested Drive / Sheets permissions.
6. Copy the Web App URL ending in `/exec`.
7. Paste that URL into `APPS_SCRIPT_URL` in `SHRAM_MITRA_CAMERA.ino`.
8. After editing Apps Script later, use **Deploy → Manage deployments → Edit → New version → Deploy**.

Open the `/exec` URL in a browser to load the dashboard. Do not open `Dashboard.html` directly from Drive or as a local file; `google.script.run` only works inside the Apps Script HTML-service page.

## 4. Test the dashboard

Open the `/exec` URL. If it fails, record the exact error text. Common issues include a mismatch in the HTML file name, an old deployed version, insufficient access, or opening the wrong URL.

## 5. Test upload

The camera firmware sends a JSON POST body with:
- `apiKey`
- `uid`
- `photoBase64`
- `mimeType`

The script saves the image in the configured Drive folder and appends the UID, name, timestamp, status, and image URL to the `Attendance` tab.

The script intentionally does not make images public by default. The dashboard may only display images for viewers who have Drive access.

## Privacy

Attendance images and student information are personal data. Use consent, restrict folder and spreadsheet access, and avoid publishing the API key or real student details in a public GitHub repository.
