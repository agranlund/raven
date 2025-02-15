
// uIPTool web interface scripts
// (c) Mariusz Buras '2019-23
// (c) Pawel Goralski '2019-23

        var CURRENT_GEMDOS_PATH;
        var FILE_LIST_REF;
        var FILE_VIEW_REF;
        var DIR_BREADCRUMB_REF;
        var DIR_LIST_REF;
        var DIR_LIST_VIEW_REF;
        var DRIVE_BUTTON_LIST_TAB_REF;
        var DRAGNDROP_AREA_REF;
        var UPLOAD_STATEINFO_REF;
        var FILE_UPLOAD_STATE_INFO_REF;
        var TOTAL_FILE_UPLOAD_PROGRESS_REF;
        var DEBUG_OUTPUT_REF;
        var GEMDOS_DRIVES_NUM;
        var INIT;
        var REQUEST_PENDING; 
        var UPLOAD_INPROGRESS;
        var UPLOAD_QUE_FINISHED;
        var UPLOAD_TOTAL_FILES;
        var UPLOAD_UPLOADED_FILES;
        var UPLOAD_CURRENT_UPLOAD_REQUEST_OBJECT;
        var TS_LAST_RENDER;
        var UPLOAD_PROCESS_LIST;
        var UPLOAD_FAILED_REQUEST_LIST;        
        var UPLOAD_COMPLETED_LIST;        
        var NO_DIRECTORIES_MSG = "No directories found.";
        var NO_FILES_MSG = "No files found.";

        var DBGLOGGER = (function () {
          return {
            log: function() {
              var args = Array.prototype.slice.call(arguments);
              console.log.apply(console, args);
           },
            warn: function() {
            var args = Array.prototype.slice.call(arguments);
            console.warn.apply(console, args);
           },
            error: function() {
            var args = Array.prototype.slice.call(arguments);
            console.error.apply(console, args);
          }
        }
        }());

        function initInternals(){
          CURRENT_GEMDOS_PATH=null;
          FILE_LIST_REF=null;
          FILE_VIEW_REF=null;
          DIR_BREADCRUMB_REF=null;
          DIR_LIST_REF=null;
          DIR_LIST_VIEW_REF=null;
          DRIVE_BUTTON_LIST_TAB_REF=null;
          DRAGNDROP_AREA_REF=null;
          UPLOAD_STATEINFO_REF=null;
          FILE_UPLOAD_STATE_INFO_REF=null;
          TOTAL_FILE_UPLOAD_PROGRESS_REF=null;
          DEBUG_OUTPUT_REF=null;
          GEMDOS_DRIVES_NUM = 0;
          INIT = true;
          REQUEST_PENDING = false; 
          TS_LAST_RENDER = 0;
          UPLOAD_INPROGRESS = false;
          UPLOAD_QUE_FINISHED = false;
          UPLOAD_TOTAL_FILES = 0;
          UPLOAD_UPLOADED_FILES = 0;
          UPLOAD_CURRENT_UPLOAD_REQUEST_OBJECT = null;
          UPLOAD_PROCESS_LIST = [];
          UPLOAD_FAILED_REQUEST_LIST = [];
          UPLOAD_COMPLETED_LIST = [];
        }

        function $id(id) {
          return document.getElementById(id);
        }

        function dnd_preventDefaults (e) {
          e.preventDefault();
          e.stopPropagation();
        }

        function dnd_highlight(e) {
          DRAGNDROP_AREA_REF.classList.add('highlight')
        }

        function dnd_unhighlight(e) {
          DRAGNDROP_AREA_REF.classList.remove('active')
        }

        function traverseFileTree(item, path) {
          path = path || "";
  
          if (item.isFile) {
              
             item.file( function(file) {
                //DBGLOGGER.log("File:", path + file.name);
                addFileToUploadQue(path,file);
              });
            
          } else if (item.isDirectory) {
              var dirReader = item.createReader();

              dirReader.readEntries( function(entries) {
                    for (var i=0; i<entries.length; ++i) {
                        traverseFileTree(entries[i], path + item.name + "/");
                    }
              });
          }
        }

  function fileLoadOnError(event){
    // DBGLOGGER.log("UI: file load error. Abort.");
    this.abort();
  }


 function uploadFiles(files){

    for (var i=0;i<files.length;++i){
      addFileToUploadQue('',files[i]);
    }

    addTotalProgressInfo(TOTAL_FILE_UPLOAD_PROGRESS_REF);
 }

  function addFileToUploadQue(path, file) {

          var gemdosName = null;
          var gemdosPath = null; 
          var current_date = new Date(); 

            // request = localpath + "/" + path + name + "?setfiledate=" + convertDateToAtariTOSFormat(date)
            if(file.webkitRelativePath === ""){
              gemdosName = convertFileNameToGemdos(path + file.name);
            }else{
              gemdosName = convertFileNameToGemdos(file.webkitRelativePath);
            }

            gemdosPath = CURRENT_GEMDOS_PATH + '/' + gemdosName;

            var request = sanitizeGemdosPath(gemdosPath);
            
            request = '/' + request;
            request += "?setfiledate=" + convertDateToAtariTOSFormat(current_date);
            
            var reader = new FileReader();
            reader.onerror = fileLoadOnError;
            reader.onload = (function(gemdosPath, request) {
              
              return function(event) {

                // insert request / data into a que
                var requestData = {
                  'filePath' : gemdosPath,
                  'request': request,
                  'data': event.target.result
                };
                ++UPLOAD_TOTAL_FILES;

                //DBGLOGGER.log("UI: Que upload http request: ", request);
                UPLOAD_PROCESS_LIST.push(requestData);
              }

            })(gemdosPath, request);

            reader.readAsArrayBuffer(file);
        }
        
        function dnd_handleDrop(e) {
         e.preventDefault();

          var path='';
          var items = e.dataTransfer.items;

          for (var i=0; i<items.length; ++i) {
            var item = items[i].webkitGetAsEntry();
            if (item != null) traverseFileTree(item, path); }

            addTotalProgressInfo(TOTAL_FILE_UPLOAD_PROGRESS_REF);

        }

        function convertFileNameToGemdos(filename){
          return filename;
        }

        function httpReadyStateToStr(id){
          switch(id){
              case 0:   return 'UNSENT';break;            //Client has been created. open() not called yet.
              case 1:   return 'OPENED';break;            //open() has been called.
              case 2:   return 'HEADERS_RECEIVED';break;  //send() has been called, and headers and status are available.
              case 3:   return 'LOADING';break;           //Downloading; responseText holds partial data.
              case 4:   return 'DONE'; break;
              default: return ''; break; 
          };
        }

        function handleUploadReqStateChange(){

          var httpReadyState = this.readyState;
          var httpStatus = this.status;
          var httpResponse = this.responseText;

          //DBGLOGGER.log("UPLOAD HTTP ready state/status ", httpReadyStateToStr(httpReadyState), httpStatus);

          if(httpReadyState == 4){

            if(httpStatus == 200 || httpStatus == 201){
               // Done. Inform the user
               //DBGLOGGER.log("Success: upload done for", UPLOAD_CURRENT_UPLOAD_REQUEST_OBJECT.filePath);
               UPLOAD_COMPLETED_LIST.push(UPLOAD_CURRENT_UPLOAD_REQUEST_OBJECT.filePath);
               destroyProgressBar();
               ++UPLOAD_UPLOADED_FILES;
            } else {
                // Error. Push currenntly processed and failed request object to fail que
                UPLOAD_FAILED_REQUEST_LIST.push(UPLOAD_CURRENT_UPLOAD_REQUEST_OBJECT);
                destroyProgressBar();
                ++UPLOAD_UPLOADED_FILES;
                //DBGLOGGER.warn("Error: upload failed of ", UPLOAD_CURRENT_UPLOAD_REQUEST_OBJECT.filePath, httpResponse, 'http status:', httpStatus);
            }
            
            UPLOAD_CURRENT_UPLOAD_REQUEST_OBJECT = null;
            UPLOAD_INPROGRESS = false;

            if(UPLOAD_QUE_FINISHED==true){
                //DBGLOGGER.log("UI: Finished upload que. Sending refresh view request.");
                destroyProgressBar();
                removeTotalProgressInfo();

                UPLOAD_TOTAL_FILES = 0;
                UPLOAD_UPLOADED_FILES = 0;
                UPLOAD_QUE_FINISHED=false;
                refreshCurrentDirView();
            }

         }

        }

        function initDragAndDropControl(){
            // Prevent default drag behaviors
            var useCapture = false;

            DRAGNDROP_AREA_REF.addEventListener('dragenter', dnd_preventDefaults, useCapture)   
            DRAGNDROP_AREA_REF.addEventListener('dragover', dnd_preventDefaults, useCapture)   
            DRAGNDROP_AREA_REF.addEventListener('dragleave', dnd_preventDefaults, useCapture)   
            DRAGNDROP_AREA_REF.addEventListener('drop', dnd_preventDefaults, useCapture)   
            
            document.body.addEventListener('dragenter', dnd_preventDefaults, useCapture);
            document.body.addEventListener('dragover', dnd_preventDefaults, useCapture);
            document.body.addEventListener('dragleave', dnd_preventDefaults, useCapture);
            document.body.addEventListener('drop', dnd_preventDefaults, useCapture);

            // Highlight drop area when item is dragged over it
            DRAGNDROP_AREA_REF.addEventListener('dragenter', dnd_highlight, useCapture);
            DRAGNDROP_AREA_REF.addEventListener('dragover', dnd_highlight, useCapture);
            DRAGNDROP_AREA_REF.addEventListener('dragleave', dnd_unhighlight, useCapture);
            DRAGNDROP_AREA_REF.addEventListener('drop', dnd_unhighlight, useCapture);

            // Handle dropped files
            DRAGNDROP_AREA_REF.addEventListener('drop', dnd_handleDrop, useCapture);
        }

        function initGlobalReferences(){
            CURRENT_GEMDOS_PATH = "";
            DIR_LIST_REF = $id("directoryList");
            FILE_LIST_REF = $id("fileList");
            FILE_VIEW_REF = $id("fileView");
            DRAGNDROP_AREA_REF = $id("fileDragAndDrop");
            DIR_LIST_VIEW_REF = $id("directoryListView");
            DIR_BREADCRUMB_REF = $id("dirBreadcrumb");
            DRIVE_BUTTON_LIST_TAB_REF = $id("driveButtonListTab");
            UPLOAD_STATEINFO_REF = $id("uploadStateInformation");
            FILE_UPLOAD_STATE_INFO_REF = $id("fileTransferProgressInfo");
            TOTAL_FILE_UPLOAD_PROGRESS_REF = $id("totalUploadStateInfo");
            DEBUG_OUTPUT_REF = $id("debugOutput");
        }

        function initFavicon(){
          var docHead = document.getElementsByTagName('head')[0];       
          var newLink = document.createElement('link');
          newLink.rel = 'shortcut icon';
          newLink.href = 'data:image/png;base64,' + img_favicon_src;
          docHead.appendChild(newLink);
        }

        function sanitizeGemdosPath(gemdosPath){
          var path = gemdosPath;
          path = path.replace(":",'/');
          path = path.replace(/\/+/g, '/').replace(/\/+$/, ''); 
          path = path.replace(/\\\\/g, '\\');
          path = path.replace(/\/\//g,'/');
          return path;
        }

        // view cleanup on reload
        function initMainView(){

          initGlobalReferences();
          initFavicon();
          initDragAndDropControl();

          if(DIR_LIST_REF!=null){
            DIR_LIST_REF.parentNode.removeChild(DIR_LIST_REF);
            DIR_LIST_REF=null;
          }
          
          if(FILE_LIST_REF!=null){
            FILE_LIST_REF.parentNode.removeChild(FILE_LIST_REF);
            FILE_LIST_REF=null;
          }

          updateBreadcrumb(CURRENT_GEMDOS_PATH);

          //hide drag and drop section
          DRAGNDROP_AREA_REF.style.display="none";        
        }

        function sortAlphabetically(a,b){
            if(a.name < b.name) { return -1; }
            if(a.name > b.name) { return 1; }
         return 0;
        }

        function processDirectoryListReq(responseText){
          var responseArray = JSON.parse(responseText);
          var dirArray = [];
          var fileArray = [];
          var tempVal = null;

          //preprocess data
          for(var i=0;i<responseArray.length;++i){
             tempVal = responseArray[i];

             if(tempVal.type.toLowerCase()=='d'){
                dirArray.push({ 
                  name: tempVal.name.toUpperCase(), 
                });
             }

             if(tempVal.type.toLowerCase()=='f'){
                fileArray.push({ 
                  name: tempVal.name.toUpperCase(), 
                  size: tempVal.size,
                  date: tempVal.date,
                  timestamp: tempVal.time
                });
             }
          }

          // sort alphabetically files / directories
          // TODO: add other options: unordered, sort by date / time, by extension
          dirArray.sort(sortAlphabetically);
          fileArray.sort(sortAlphabetically);

          updateDirectoryViewUI(dirArray);
          updateFileViewUI(fileArray);

          responseArray = [];
          dirArray = [];
          fileArray = [];

          REQUEST_PENDING = false;
        }

            // TOS drives: A/B(floppy), C-P
            // MultiTOS: A/B,C-Z + U 
            // files/directories 8+3
            // valid GEMDOS characters:  
            // A-Z, a-z, 0-9
            // ! @ # $ % ^ & ( )
            // + - = ~ ` ; ' " ,
            // < > | [ ] ( ) _ 
            // . - current dir
            // .. parent dir

        function requestChangeDrive(driveLetter){
            // request dir status
            sendHttpReq(location.host + '/' + driveLetter,'dir', 'GET', processDirectoryListReq);
            
            //todo: check request result
            CURRENT_GEMDOS_PATH = driveLetter + ':'; 
            updateBreadcrumb(CURRENT_GEMDOS_PATH);
            DRAGNDROP_AREA_REF.style.display="block";        
        }

        // get gemdos path and generate links based on folder hierarchy
        // with embedded change directory requests
        function updateBreadcrumb(GemdosPath) {
          
          var pathArray = sanitizeGemdosPath(GemdosPath).replace(/\/$/, "").split('/');
          var btnLink='';            
          
          DIR_BREADCRUMB_REF.innerHTML='';
          var accuStr='';

          for(var elem=0;elem<pathArray.length;++elem){
            accuStr += pathArray[elem]; 
            btnLink='<button type="button" class="BcrmbButton" name="' + accuStr + '" onclick="handleBcOnClick(this.name)">' +'<span>'+ pathArray[elem] +'</span>'+ '</button>';
            DIR_BREADCRUMB_REF.innerHTML += btnLink;
            
            if(elem==0)
              DIR_BREADCRUMB_REF.innerHTML += '&#32;&#58;&#32;'
            else
              DIR_BREADCRUMB_REF.innerHTML += '&#32;&#47;&#32;'

            accuStr += '/';

          }

        }

        function requestChangePath(pathStr){
              //request dir status
              sendHttpReq(location.host + '/'+ pathStr,'dir','GET', processDirectoryListReq);

              //todo: check request result
              CURRENT_GEMDOS_PATH = pathStr;
              updateBreadcrumb(CURRENT_GEMDOS_PATH);
        }

        function refreshCurrentDirView(){
              var currDir = sanitizeGemdosPath(CURRENT_GEMDOS_PATH);
              sendHttpReq(location.host + '/'+ currDir,'dir','GET', processDirectoryListReq);
        }


        function requestChangeDir(dirStr){
            var pathPrefix='';

            if(dirStr=='ROOT'){

              var pathArray = CURRENT_GEMDOS_PATH.replace(/\/$/, "").split('/');
              
              CURRENT_GEMDOS_PATH='';
              
              if(pathArray.length==2){
                  CURRENT_GEMDOS_PATH = pathArray[0];
              }else{
                for(var i=0;i<(pathArray.length-1);++i){
                  CURRENT_GEMDOS_PATH = CURRENT_GEMDOS_PATH + pathArray[i] + '/' ;
                }
              }

              pathPrefix = sanitizeGemdosPath(CURRENT_GEMDOS_PATH);

              //request dir status
              sendHttpReq(location.host + '/' + pathPrefix ,'dir', 'GET', processDirectoryListReq);
              updateBreadcrumb(CURRENT_GEMDOS_PATH);
          
            }else{

              pathPrefix = sanitizeGemdosPath(CURRENT_GEMDOS_PATH);

              //request dir status
              sendHttpReq(location.host + '/' + pathPrefix + '/'+ dirStr,'dir', 'GET', processDirectoryListReq);

              //todo: check request result
              CURRENT_GEMDOS_PATH = CURRENT_GEMDOS_PATH + '/' + dirStr;
              updateBreadcrumb(CURRENT_GEMDOS_PATH);
            }
            
        }

        function processFileDownloadReq(httpResponse)
        {
          //DBGLOGGER.log("[processFileDownloadReq] handler");
        }

        function processCreateNewDirectoryReq(httpResponse)
        {
          window.alert("Directory created!");
          refreshCurrentDirView();
        }

        function requestFileDownload(btn){
            
            var pathPrefix = CURRENT_GEMDOS_PATH;
            pathPrefix = pathPrefix + '/' + btn.name;
            pathPrefix = sanitizeGemdosPath(pathPrefix);
 
            // request file download
            sendHttpReq(location.host + '/' + pathPrefix,'', 'GET', processFileDownloadReq);
        } 

        String.prototype.replaceAt = function(index, replacement) {
          return this.substring(0, index) + replacement + this.substring(index + replacement.length);
        }

        function requestCreateNewFolder(dirName)
        {
          // create new folder http request
          var currentPath = CURRENT_GEMDOS_PATH;

          if(dirName.length>8)
          {
            dirName = dirName.substring(0,12).replaceAt(8,'.');
          }

          dirName = dirName.toUpperCase();

          var newFolderRequest = location.host + '/' + currentPath + '/' + dirName;
          newFolderRequest = sanitizeGemdosPath(newFolderRequest);
          sendHttpReq(newFolderRequest, 'newfolder', 'GET', processCreateNewDirectoryReq);
        }

        function handleDriveOnClick(){
          requestChangeDrive(this.name);            
          return false;
        }

        function createDriveButtons(node, DriveArray){
          var buttonStr = null;

          GEMDOS_DRIVES_NUM = DriveArray.length;

          for(var i=0;i<DriveArray.length;++i){

             buttonStr = DriveArray[i].name.toUpperCase();
             var button = document.createElement("button");
             var span = document.createElement("span");
             var textNode = document.createTextNode(buttonStr + ':');

             var img = document.createElement('img');
 
             if(buttonStr=='A' || buttonStr=='B'){
                img.alt = "file icon floppy drive";
                img.src = 'data:image/png;base64,' + img_floppy_active_src;
             } else {
                img.alt = "file icon hard drive";
                img.src = 'data:image/png;base64,' + img_hd_active_src;
             }
 
             node.appendChild(img);
             span.appendChild(textNode);
             button.appendChild(img);
             button.appendChild(span);

             button.type='button';
             button.name = buttonStr;
             button.onclick = handleDriveOnClick;
             button.className ='HdButton';  
             node.appendChild(button);
          };

            if(INIT==true){

              if(GEMDOS_DRIVES_NUM > 2){
                requestChangeDrive('C');
              }else if( (GEMDOS_DRIVES_NUM <= 2) && (GEMDOS_DRIVES_NUM>0) ){
                requestChangeDrive(DriveArray[0].name.toUpperCase());
              }

              INIT = false;

            }
        }

        function updateDriveViewUI(DriveArray){
          var div = document.createElement("div");
          div.id="DriveButtonGroup"
          div.innerHTML = '';
          createDriveButtons(div, DriveArray);            
          
          DRIVE_BUTTON_LIST_TAB_REF.appendChild(div);
        }

        // directory view generation
        function handleDirectoryOnClick(){
          if(REQUEST_PENDING!=true){
            REQUEST_PENDING=true;
            requestChangeDir(this.name);
          }

          return false;
        }

        function handleBcOnClick(name){
          if(REQUEST_PENDING!=true){
            REQUEST_PENDING=true;
            requestChangePath(name);
          }

          return false;
        }

        function handleNewFolderOnClick()
        {
          var newFolderName = prompt("Create new folder in:\n" + CURRENT_GEMDOS_PATH + "\nEnter folder name (8+3), longer names will be truncated", "");
          
          if (newFolderName === null)
          {
              return; // cancel
          }

          if(newFolderName == "")
          {
            window.alert("Error: Folder creation failed. Name cannot be empty!");
          }
          else
          {
            requestCreateNewFolder(newFolderName);  
          }
        }

        function createDirectoryEntries(node, DirectoryArray){
          var directoryStr = null;
          var directoryReqStr=null;
          
          if(CURRENT_GEMDOS_PATH.length > 2){
            var button = document.createElement("button");
            var span = document.createElement("span");
            var textNode = document.createTextNode('..');

            var img = document.createElement('img');
            img.alt = "folder closed icon";
            img.src = 'data:image/png;base64,' + img_dir_close_src;
            
            span.appendChild(textNode);
            button.appendChild(img);
            button.appendChild(span);

            button.type='button';
            button.name = 'ROOT';
            button.className ='directoryButton';
            button.onclick = handleDirectoryOnClick;
            
            node.appendChild(button);
          }

          for(var i=0;i<DirectoryArray.length;++i){
                directoryStr = DirectoryArray[i].name;
                directoryReqStr=directoryStr;
                if(directoryStr=='ROOT') directoryStr='..';
                if(directoryReqStr=='ROOT') directoryReqStr='';
                
                var button = document.createElement("button");
                var span = document.createElement("span");
                var textNode = document.createTextNode(directoryStr);
                var requestStr = directoryReqStr;

                var img = document.createElement('img');
                img.alt = "folder closed icon";
                img.src = 'data:image/png;base64,' + img_dir_close_src;
                span.appendChild(textNode);
                button.appendChild(img);
                button.appendChild(span);

                button.type='button';
                button.name = requestStr;
                button.className ='directoryButton';
                button.onclick = handleDirectoryOnClick;

                node.appendChild(button);
          };
           
           if(DirectoryArray.length==0) {
              var tn = document.createTextNode(NO_DIRECTORIES_MSG);
              node.appendChild(tn);    
           }
                 
        }

        function updateDirectoryViewUI(DirJsonArray)
        {
          var div = document.createElement("div");
          div.id="directoryList";
          createDirectoryEntries(div,DirJsonArray);

          if(DIR_LIST_REF!=null){
            DIR_LIST_REF.parentNode.removeChild(DIR_LIST_REF);
            DIR_LIST_REF=null;
          }
          
          DIR_LIST_VIEW_REF.insertBefore(div, DIR_LIST_VIEW_REF.childNodes[0]); 
          DIR_LIST_REF=$id("directoryList");

          // append new folder button
          var newFolderButton = document.getElementsByName("CREATENEWFOLDER");
          if(newFolderButton==null || newFolderButton=="" || newFolderButton.length==0)
          {
            var button = document.createElement("button");
            var span = document.createElement("span");
            var textNode = document.createTextNode('Create new folder');

            var img = document.createElement('img');
            img.alt = "folder closed icon";
            img.src = 'data:image/png;base64,' + img_folder_add_src;
            
            span.appendChild(textNode);
            button.appendChild(img);
            button.appendChild(span);

            button.type='button';
            button.name = 'CREATENEWFOLDER';
            button.className ='directoryButton';
            button.onclick = handleNewFolderOnClick;

            var textNode = document.createTextNode('File operations');

            var div = document.createElement("div");
            div.id = "fileOperationsList";
            div.appendChild(textNode);
            div.appendChild(button);
            DIR_LIST_VIEW_REF.parentNode.appendChild(div);
          }
        }

        // file view generation
        function handleFileOnClick(){
            requestFileDownload(this);
            return false;            
        }

        function createFileDownloadLink(node, fileName){
            var a = document.createElement('a');
            var span = document.createElement('span');           
            var linkText = document.createTextNode(fileName);
            
            var pathPrefix = CURRENT_GEMDOS_PATH;
            pathPrefix += '/' + fileName;
            
            pathPrefix = sanitizeGemdosPath(pathPrefix);
            
            a.href = pathPrefix;  
            a.download = fileName;

            span.appendChild(linkText)
            a.appendChild(span);
            node.appendChild(a);
        }

        function padDigits(num, size)
        {
          var s = "00" + num;
          return s.substr(s.length-size);
        }
        
        function formatFileDate(dateString)
        {
          var dateArray = dateString.split("/");
          var d = padDigits(dateArray[0],2);
          var m = padDigits(dateArray[1],2);
          var y = dateArray[2];
          return d + '.' + m + '.' + y;
        }

        function formatFileTime(timeString)
        {
          var hourArray = timeString.split(":");
          var h = padDigits(hourArray[0],2);
          var m = padDigits(hourArray[1],2);
          var s = padDigits(hourArray[2],2);
          return h + ':' + m + '.' + s;
        }

        function formatFileSize(filesize)
        {
          var ffs;
          if(filesize!=0)
          {
            var base = Math.log(filesize) / Math.log(1024);
            var floorBase = Math.floor(base);
            var suffix = new Array("B", "kiB", "MiB", "GiB", "TiB");
          
            if(floorBase==0)
            {
              ffs = Math.pow(1024, base - floorBase).toFixed(0)
              ffs += ' ' + suffix[floorBase];
            }
            else
            {
                ffs = Math.pow(1024, base - floorBase).toFixed(1)
                ffs += ' ' + suffix[floorBase] + ' (' + filesize + ' B)';
            }
          }
          else
          {
            ffs = filesize + ' B';
          }

          return ffs;
        }

        function createFileEntries(node, FileArray){
          var fileStr = null;
          var fileSize = 0;
          var fileDate="";
          var fileTime="";

          var tableDiv = document.createElement('div');
          tableDiv.classList.add('divTable');
          tableDiv.classList.add('blueTable');
          
          var tableHead = document.createElement('div');
          tableHead.classList.add('divTableHeading');
         
          var tableRow = document.createElement('div');
          tableRow.classList.add('divTableRow');
          
          var tableHead1 = document.createElement('div');
          var tableHead2 = document.createElement('div');
          var tableHead3 = document.createElement('div');
          var tableHead4 = document.createElement('div');

          tableHead1.classList.add('divTableHead');
          tableHead2.classList.add('divTableHead');
          tableHead3.classList.add('divTableHead');
          tableHead4.classList.add('divTableHead');

          var headNameText1=document.createTextNode('filename');
          var headNameText2=document.createTextNode('size');
          var headNameText3=document.createTextNode('date');
          var headNameText4=document.createTextNode('timestamp');
 
          tableHead1.appendChild(headNameText1);
          tableHead2.appendChild(headNameText2);
          tableHead3.appendChild(headNameText3);
          tableHead4.appendChild(headNameText4);

          tableRow.appendChild(tableHead1);
          tableRow.appendChild(tableHead2);
          tableRow.appendChild(tableHead3);
          tableRow.appendChild(tableHead4);

          tableHead.appendChild(tableRow);
          tableDiv.appendChild(tableHead);

          var tableBody = document.createElement('div');
          tableBody.classList.add('divTableBody');
          tableDiv.appendChild(tableBody);
          
          /* add entries to tableBody */
          node.appendChild(tableDiv);

          for(var i=0;i<FileArray.length;++i){
             var fileRow = document.createElement('div');
             fileRow.classList.add('divTableRow');

             var cell1 = document.createElement('div');
             var cell2 = document.createElement('div');
             var cell3 = document.createElement('div');
             var cell4 = document.createElement('div');

             cell1.classList.add('divTableCell');
             cell2.classList.add('divTableCell');
             cell3.classList.add('divTableCell');
             cell4.classList.add('divTableCell');

             fileRow.appendChild(cell1);
             fileRow.appendChild(cell2);
             fileRow.appendChild(cell3);
             fileRow.appendChild(cell4);

             tableBody.appendChild(fileRow);
             
             fileStr = FileArray[i].name.toUpperCase();
             fileSize = FileArray[i].size;
             fileDate = FileArray[i].date;
             fileTime = FileArray[i].timestamp;

             var div = document.createElement('div');
             div.id = "fileEntryInfo";

             var img = document.createElement('img');
             img.alt = "file icon generic";
             img.src = 'data:image/png;base64,' + img_file_generic_src;
             div.appendChild(img);

             cell1.appendChild(div);
             cell2.innerHTML = formatFileSize(fileSize);
             cell3.innerHTML = formatFileDate(fileDate);
             cell4.innerHTML = formatFileTime(fileTime);
             
             createFileDownloadLink(div,fileStr);
          }

          if(FileArray.length==0) 
              node.innerHTML += NO_FILES_MSG + '<br/>';
        }

        function updateFileViewUI(FileJsonArray){

          var div = document.createElement("div");
          div.id="fileList"

          createFileEntries(div, FileJsonArray)

          if(FILE_LIST_REF!=null){
            FILE_LIST_REF.parentNode.removeChild(FILE_LIST_REF);
            FILE_LIST_REF=null;
          }

          FILE_VIEW_REF.appendChild(div);
          FILE_LIST_REF=$id("fileList");
        }

        function processDriveListReq(responseText) {
          var DriveArray = JSON.parse(responseText);
          updateDriveViewUI(DriveArray);  
        }

        // send async http request 
        function sendHttpReq(addr, params, requestType, requestHandler){

            var xhr = new XMLHttpRequest();
            
            if(params==null || params.length==0){
              params = '';
            } else{
              params = '/?' + params;
            }
            
            xhr.onreadystatechange = function() { 
              if (xhr.readyState == 4 && xhr.status == 200)
                requestHandler(xhr.responseText);              
            }

            xhr.open(requestType, 'http://' + addr + params, true);
            xhr.send();
        }

        function updateDriveListReq(){
            sendHttpReq(location.host,'dir', 'GET', processDriveListReq);            
        }

    // This returns at least 32bit int with date encoded for DOSTIME struct
    // typedef struct
    // {
    //      uint16_t     time;  /* Time like Tgettime */
    //      uint16_t     date;  /* Date like Tgetdate */
    // } DOSTIME;
    
    function convertDateToAtariTOSFormat(date) {
        var tosYear = (date.getFullYear() - 1980);

        tosYear = tosYear < 0 ? 0 : tosYear;
        tosYear = tosYear > 119 ? 119 : tosYear;
        var tosDay = date.getDate();
        var tosMonth = date.getMonth() + 1;
        var tosDate = (tosYear) << 9 | (tosMonth << 5) | tosDay;
        var tosSeconds = date.getSeconds() / 2;
        var tosMinutes = date.getMinutes();
        var tosHours = date.getHours();
        var tosTime = (tosHours << 11) | (tosMinutes<<5) | tosSeconds;
        return ((tosTime<<16) | tosDate) >>> 0;
     }

     function createProgressBar(node, filename){
        var progress = document.createElement("progress");
        var span = document.createElement("span");
        var textNode = document.createTextNode(filename);

        progress.id = "fileUploadProgresBar";
        span.appendChild(textNode);
        node.appendChild(progress);
        node.appendChild(span);
    
        progress.value = 0;
        progress.max = 100;
     }

     function destroyProgressBar(){
        while (FILE_UPLOAD_STATE_INFO_REF.hasChildNodes()) {
          FILE_UPLOAD_STATE_INFO_REF.removeChild(FILE_UPLOAD_STATE_INFO_REF.lastChild);
        }
     }

     function addTotalProgressInfo(node){
       UPLOAD_UPLOADED_FILES = 0;

       var text = 'Uploaded / total [ ' + UPLOAD_UPLOADED_FILES + ' / ' + UPLOAD_TOTAL_FILES + ' ]';
       var textNode = document.createTextNode(text);
       node.appendChild(textNode);
     }

     function updateTotalProgressInfo(node){
      if(node.lastChild!=null){
       var text = 'Uploaded / total [ ' + UPLOAD_UPLOADED_FILES + ' / ' + UPLOAD_TOTAL_FILES + ' ]';
       node.lastChild.nodeValue = text;
      }
     }

     function removeTotalProgressInfo(){
      while (TOTAL_FILE_UPLOAD_PROGRESS_REF.hasChildNodes()) {
          TOTAL_FILE_UPLOAD_PROGRESS_REF.removeChild(TOTAL_FILE_UPLOAD_PROGRESS_REF.lastChild);
        }
     }

     function onUploadProgress(evt){
        var progressBar = $id("fileUploadProgresBar");
        progressBar.value = evt.loaded * 100 / evt.total;
        // DBGLOGGER.log("File: ", UPLOAD_CURRENT_UPLOAD_REQUEST_OBJECT.filePath,", upload progress: ",(evt.loaded * 100 / evt.total) || 100);
     }

     function sendUploadHttpRequest(uploadRequestObject){
         // insert blob into http request que
         var xhr = new XMLHttpRequest;
         var uploadBinaryBlob = new Blob([uploadRequestObject.data], {type: 'application/octet-binary'});

         xhr.onreadystatechange = handleUploadReqStateChange;
         xhr.upload.onprogress = onUploadProgress;
         xhr.open('POST', uploadRequestObject.request, true);
         xhr.send(uploadBinaryBlob);

         // create progress bar 
         createProgressBar(FILE_UPLOAD_STATE_INFO_REF, uploadRequestObject.filePath);

         //DBGLOGGER.log("Submitted upload request: ",uploadRequestObject.request);
     }

     // update status info     
     function updateUploadStateInformation(dt){

      if(UPLOAD_PROCESS_LIST==null||
        UPLOAD_FAILED_REQUEST_LIST==null||
        UPLOAD_COMPLETED_LIST==null) return;

      while (UPLOAD_STATEINFO_REF.hasChildNodes()) {
          UPLOAD_STATEINFO_REF.removeChild(UPLOAD_STATEINFO_REF.lastChild);
      }

      var divProgress = document.createElement("div");
      divProgress.id = 'progressQue';

      divProgress.classList.add('transferStateColumn');
      divProgress.classList.add('progressQue');
      divProgress.innerHTML = "<h3>Processing que:</h3>";
      
      var divFailed = document.createElement("div");
      divFailed.id = 'failedQue';
      divFailed.classList.add('transferStateColumn');
      divFailed.classList.add('failedQue');
      divFailed.innerHTML = "<h3>Failed:</h3>";      
      
      var divCompleted = document.createElement("div");
      divCompleted.id = 'completedList';
      divCompleted.classList.add('transferStateColumn');
      divCompleted.classList.add('completedList');
      divCompleted.innerHTML = "<h3>Completed:</h3>";
      
      UPLOAD_STATEINFO_REF.appendChild(divProgress);
      UPLOAD_STATEINFO_REF.appendChild(divFailed);
      UPLOAD_STATEINFO_REF.appendChild(divCompleted);      

      for(var i=0;i<UPLOAD_PROCESS_LIST.length;++i){
        divProgress.innerHTML +=  UPLOAD_PROCESS_LIST[i].filePath+"<br/>";      
       }  
       
       for(var i=0;i<UPLOAD_FAILED_REQUEST_LIST.length;++i){
        divFailed.innerHTML +=  UPLOAD_FAILED_REQUEST_LIST[i].filePath+"<br/>";      
       }  
       
       for(var i=0;i<UPLOAD_COMPLETED_LIST.length;++i){
        divCompleted.innerHTML +=  UPLOAD_COMPLETED_LIST[i]+"<br/>";             
       }  

       updateTotalProgressInfo(TOTAL_FILE_UPLOAD_PROGRESS_REF);

     } 

     function clearCompletedList(){
        UPLOAD_COMPLETED_LIST=[];
     }

     function requeFailedTransfers(){
      for(var i=0;i<UPLOAD_FAILED_REQUEST_LIST.lenght;++i){
        UPLOAD_PROCESS_LIST.push(UPLOAD_FAILED_REQUEST_LIST[i]);
        ++UPLOAD_TOTAL_FILES;
      }
      UPLOAD_FAILED_REQUEST_LIST=[];
     }

    // Update 
    function update(dt) {
        if((UPLOAD_PROCESS_LIST.length != 0) && (UPLOAD_INPROGRESS!=true)){
          UPLOAD_INPROGRESS = true; // get first from array
          UPLOAD_CURRENT_UPLOAD_REQUEST_OBJECT = UPLOAD_PROCESS_LIST.shift();
          sendUploadHttpRequest(UPLOAD_CURRENT_UPLOAD_REQUEST_OBJECT);

          if(UPLOAD_PROCESS_LIST.length==0){
            // pushed all requests, refresh UI view after request finishes
            UPLOAD_QUE_FINISHED=true;
          }
        } 
    }

    function mainLoop(timestamp) {
      var dt = timestamp - TS_LAST_RENDER;
      update(dt);
      updateUploadStateInformation(dt);
      TS_LAST_RENDER = timestamp
      window.requestAnimationFrame(mainLoop)
    }

    function startup(){
        //DBGLOGGER.log("[STARTUP] Page reload event");
        initInternals();
        initMainView();
        updateDriveListReq();
        window.requestAnimationFrame(mainLoop);
    }

     // debug output functions
     function DebugOutput(msg) {
        var m = DEBUG_OUTPUT_REF;
        m.innerHTML = msg + m.innerHTML;
    }
