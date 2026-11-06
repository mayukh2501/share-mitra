/*
 * SHRAM MITRA — Google Apps Script backend
 *
 * Put this file and Dashboard.html in the SAME Apps Script project.
 * Set a strong private API key before deployment.
 */

const API_KEY = "CHANGE_THIS_TO_A_LONG_RANDOM_SECRET";
const SPREADSHEET_ID = "1O23bEciw68S6AJzO9--faOnqfGIHgn-zvgj6DEAJkX0";
const DRIVE_FOLDER_ID = "1nmCi334d7ZRbGpJfqxMmyZYaGuH6tZVS";
const SHEET_NAME = "Attendance";

function doGet() {
  return HtmlService.createHtmlOutputFromFile("Dashboard")
    .setTitle("SHRAM MITRA | Attendance Dashboard")
    .setXFrameOptionsMode(HtmlService.XFrameOptionsMode.DEFAULT);
}

function doPost(e) {
  try {
    if (!e || !e.postData || !e.postData.contents) {
      return jsonResponse({ success: false, message: "Empty request body" });
    }

    const payload = JSON.parse(e.postData.contents);
    if (payload.apiKey !== API_KEY) {
      return jsonResponse({ success: false, message: "Unauthorized" });
    }

    const uid = String(payload.uid || "").trim();
    const photoBase64 = String(payload.photoBase64 || "");
    const mimeType = String(payload.mimeType || "image/jpeg");

    if (!uid || !photoBase64) {
      return jsonResponse({ success: false, message: "Missing UID or photo data" });
    }

    const folder = DriveApp.getFolderById(DRIVE_FOLDER_ID);
    const bytes = Utilities.base64Decode(photoBase64);
    const safeUid = uid.replace(/[^A-Za-z0-9_-]/g, "_");
    const timestamp = new Date();
    const filename = "SHRAM_MITRA_" + safeUid + "_" +
      Utilities.formatDate(timestamp, Session.getScriptTimeZone(), "yyyyMMdd_HHmmss") + ".jpg";
    const blob = Utilities.newBlob(bytes, mimeType, filename);
    const file = folder.createFile(blob);

    // Keep the file private by default. Share the Drive folder only with intended viewers.
    const photoUrl = file.getUrl();

    const ss = SpreadsheetApp.openById(SPREADSHEET_ID);
    let sheet = ss.getSheetByName(SHEET_NAME);
    if (!sheet) {
      sheet = ss.insertSheet(SHEET_NAME);
      sheet.appendRow(["Timestamp", "Student Name", "RFID UID", "Status", "Photo URL"]);
    }

    const studentName = lookupStudentName(uid);
    sheet.appendRow([timestamp, studentName, uid, "Present", photoUrl]);

    return jsonResponse({
      success: true,
      message: "Attendance saved",
      uid: uid,
      studentName: studentName,
      timestamp: timestamp.toISOString(),
      photoUrl: photoUrl
    });
  } catch (err) {
    return jsonResponse({ success: false, message: String(err) });
  }
}

function lookupStudentName(uid) {
  // Add approved RFID UID-to-name mappings here, for example:
  // const students = { "A1B2C3D4": "Demo Student" };
  const students = {};
  return students[uid] || "Unregistered Student";
}

function getAttendance() {
  const ss = SpreadsheetApp.openById(SPREADSHEET_ID);
  let sheet = ss.getSheetByName(SHEET_NAME);

  if (!sheet) {
    sheet = ss.insertSheet(SHEET_NAME);
    sheet.appendRow(["Timestamp", "Student Name", "RFID UID", "Status", "Photo URL"]);
    return [];
  }

  const values = sheet.getDataRange().getValues();
  if (values.length <= 1) return [];

  return values.slice(1).reverse().map(function(row) {
    const date = row[0] instanceof Date ? row[0] : new Date(row[0]);
    return {
      timestamp: isNaN(date.getTime()) ? String(row[0]) :
        Utilities.formatDate(date, Session.getScriptTimeZone(), "dd MMM yyyy, hh:mm:ss a"),
      studentName: String(row[1] || ""),
      uid: String(row[2] || ""),
      status: String(row[3] || ""),
      photoUrl: String(row[4] || "")
    };
  });
}

function jsonResponse(data) {
  return ContentService
    .createTextOutput(JSON.stringify(data))
    .setMimeType(ContentService.MimeType.JSON);
}
