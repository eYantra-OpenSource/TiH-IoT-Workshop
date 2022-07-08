function doGet(e){
  //
  // open the spreadsheet
  var ss = SpreadsheetApp.getActive();

  // use the 'id' parameter to differentiate between sheet
  var sheet = ss.getSheetByName(e.parameter["id"]);

  // extract headers
  // getRange accepts row, col, number_of_rows and num_of_cols as argument
  // getLastColumn returns the position of the last column that has content
  var headers = sheet.getRange(1, 1, 1, sheet.getLastColumn()).getValues()[0];

  // store the position of the last row
  var lastRow = sheet.getLastRow();

  var cell = sheet.getRange('a1');
  var col = 0;
  var d = new Date();

  for (i in headers)
  {

    // loop through the headers and if a parameter name matches the header name insert the value
    if (headers[i] == "Timestamp")
    {
      val = d.toDateString() + ", " + d.toLocaleTimeString();
    }
    else
    {
      val = e.parameter[headers[i]]; 
    }

    // append data to the last row
    cell.offset(lastRow, col).setValue(val);
    col++;
  }
  
  //

  //Checks the last logged data in the sheet, sends email if data greater is than threshold

  var sh = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  var lastRow = sh.getLastRow();
  var lastCol = sh.getLastColumn();
  var data_h = sh.getRange(lastRow, lastCol-1).getValue();
  var data_ldr = sh.getRange(lastRow, lastCol).getValue();
  data_ldr = parseFloat(data_ldr);

  ////////////////////////Compare Humidity/////////////////////////////
   var sheet_thresh_h = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Threshold");
    let startRow_thresh_h = 2; // First row of data to process
    let numRows_thresh = 2; // Number of rows to process
    // Fetch the range of cells A2:B3
    const dataRange_thresh_h = sheet_thresh_h.getRange(startRow_thresh_h, 1, sheet_thresh_h.getLastRow()-1, 3);
    // Fetch values for each row in the Range.
    const data_thresh_h = dataRange_thresh_h.getValues();
    Logger.log(data_thresh_h);
    for (let row1 of data_thresh_h) 
    {
      const Threshold_h = row1[0];
      const Threshold_ldr_low = row1[1];
      const Threshold_ldr_high = row1[2];
      Logger.log(typeof data_h);
      Logger.log(typeof data_ldr);
   
    var text = "";
    ////////////////////////////////////////////////////
    if(data_h > Threshold_h)     //set your threshold here
    {
      text = "FAN on|";
    }
    else
    {
      text = "FAN off|";
    }


    if(data_ldr < Threshold_ldr_high)     //set your threshold here
    {
      text += "LED cool";
    }
    else if(data_ldr < Threshold_ldr_low)
    {
      text += "LED warm";
    }
    // @ts-ignore
    else
    {
      text += "LED off";
    }
    return ContentService.createTextOutput(text);
    }

}
