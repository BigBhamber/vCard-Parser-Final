// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    var getContent = [];


    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/endpoint1',   //The server endpoint we are connecting to
        data: {
            stuff: "Value 1",
            junk: "Value 2"
        },
        success: function (data) {

            /*  Do something with returned object
                Note that what we get is an object, not a string,
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            $('#blah').html("On page load, received string '"+data.stuff+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data);
            getContent = data;
            //createDropdown(data);
            createLogTab();
            createDropdown();
            resetParamText();

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log('table and drop down unable to be created due to error');
             alert("table and drop down unable to be created due to error");
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#uploadForm').submit(function(e){
        e.preventDefault();

        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
        type: 'post',
         url: '/makeSelect',
         dataType: 'json',
         contentType : 'application/json',
         data: '{}',
         success: function(data) {
             //console.log('Uploaded file');
             var uploadedVcard = $('#uploadForm').find('input[name="uploadFile"]').val();
             var y = uploadedVcard.split("\\").pop();
             if(y.includes(".vcf"))
             {

                 console.log('Uploaded file: '+y);
                 addToTable(uploadedVcard);
             }
             else
             {
                 console.log("invalid file");
                 alert("File is invalid, please use a file with a .vcf extension");
             }

         },
         error: function(error) {
             console.log('Error while uploading file.');
              alert("Error while uploading files");
         }
            //Create an object for connecting to another waypoint

        });
    });
    $('#vCard-files').change(function(){

        var selectedVcard = $(this).val();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
        type: 'get',
         url: '/SelectCard',
         dataType: 'json',
         contentType : 'application/json',
         data: '{}',
         success: function(data) {
             console.log('selected vcard:'+selectedVcard+' displaying Content table for card');

                 displayCard(selectedVcard);


         },
         error: function(error) {
             console.log('error occured when trying to display content of Card');
              alert("Error in dislaying card content");
         }
            //Create an object for connecting to another waypoint

        });
    });
    $('#AddingProp').submit(function(e){
        e.preventDefault();

        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
        type: 'post',
         url: '/newProp',
         dataType: 'json',
         contentType : 'application/json',
         data: '{}',
         success: function(data) {
             var selectedVcard = $('#vCard-files').val();
             var name = $('#AddingProp').find('input[name="pName"]').val();
             var valueProp = $('#AddingProp').find('input[name="pVal"]').val();
             console.log('Adding property: '+name+":"+valueProp);

             newProp(name,valueProp);
             addPropTable(selectedVcard,prop);
         },
         error: function(error) {
             console.log('Error occured while adding property');
              alert("Error in adding property");
         }
            //Create an object for connecting to another waypoint

        });
    });
    $('#createCard').submit(function(e){
        e.preventDefault();

        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
        type: 'post',
         url: '/newCard',
         dataType: 'json',
         contentType : 'application/json',
         data: '{}',
         success: function(data) {

             var cardVal = $('#createCard').find('input[name="CardName"]').val();
             var fnValEntry = $('#createCard').find('input[name="fnName"]').val();
             console.log('Creating card with: '+cardVal+' and: '+fnValEntry);

             createCardFunc(cardVal,fnValEntry);

         },
         error: function(error) {
             console.log('Error in file information.');
              alert("Error in file information");
         }
            //Create an object for connecting to another waypoint

        });
    });

    $('#showParameters').submit(function(e){
        e.preventDefault();

        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
        type: 'get',
         url: '/displayParam',
         dataType: 'json',
         contentType : 'application/json',
         data: '{}',
         success: function(data) {
             var x = $('#showParam').find('input[name="ParamRow"]').val();
             getParams(x);


         },
         error: function(error) {
             console.log('Error in accessing Parameters.');
             alert("Error in accessing Parameters");
         }
            //Create an object for connecting to another waypoint

        });
    });
    $('#editProperty').submit(function(e){
        e.preventDefault();

        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
        type: 'post',
         url: '/propChange',
         dataType: 'json',
         contentType : 'application/json',
         data: '{}',
         success: function(data) {
             console.log('Editing property');
             var rowChange = $('#editProperty').find('input[name="rowNumber"]').val();
             var editval = $('#editProperty').find('input[name="EditedVal"]').val();
             ChangeProp(rowChange, editval);


         },
         error: function(error) {
             console.log('Error in editing property');
             alert("Edit property Error.");
         }
            //Create an object for connecting to another waypoint

        });
    });


});
/*
function createDropdown(data) {

    var dropBox = document.getElementById('vCard-files');
    var option;
    //loops through and creates dropDown based on files in server.
    for(var i = 0; i < data.length; i++) {
        option = ("<option value=\""+data[i].fileName+"\">"+data[i].fileName+"</option>");
        dropBox.innerHTML += option;
    }
    // if server side is empty, will display no files.
    if(data.length === 0) {
        option = ("<option value=\"No Files\">No Files</option>");
        dropBox.innerHTML += option;
        console.log('No Files in server');
    }

}
*/

function resetParamText()
{
    document.getElementById('paramOutput').value = "This is where Parameters will be displayed";
}
function ChangeProp(rowChange, editval)
{
    var length = document.getElementById("view-body").rows.length;

    if (rowChange>length)
    {
        //out of range.
        console.log('specified row, is out of range');
        alert("Chosen row is not within valid range. remember the range is from 0 to (the last property # -1)");
    }
    else if (rowChange<0)
    {
        console.log('specified row, is out of range');
        alert("Row is out of range,remember range is 0- the last property #-1");
    }
    else
    {
        var checkNumvals = document.getElementById("view-body").rows[rowChange].cells[2].innerHTML;
        if (checkNumvals.includes(";"))
        {
            //cannot edit property. More than 1 value
            console.log('Selected property contains multiple values,Cannot be edited');
            alert("Selected Property contains multiple values, Cannot be edited");
        }
        else
        {
            var testCount = 0;
            var replace;
            if(editval.includes(";"))
            {
                replace = editval.replace(/;/g, ", ");
                testCount =1;
            }
            if (testCount === 0)
            {
                document.getElementById("view-body").rows[rowChange].cells[2].innerHTML = '<td>'+editval+'</td>';
            }
            else
            {
                document.getElementById("view-body").rows[rowChange].cells[2].innerHTML = '<td>'+replace+'</td>';
            }

        }
        // row is valid, now need to validate
    }
}
function getParams(x)
{
    var length = document.getElementById("view-body").rows.length;
    var paramName = ["Type","Value","PREF"]
    var paramsVals = ["Work","URI","1"]
    var output;

    if (x>length)
    {
        //invalid row.
        console.log('Specified row, is not within range.');
        alert("Chosen row is not within valid range. remember the range is from 0 to (the last property # -1)");
    }
    else if(x<0)
    {
        console.log('Specified row, is not within range,remember row input is -1 from the row in the table .');
        alert("Chosen row is not within valid range. remember the range is from 0 to (the last property # -1)");
    }
    else
    {
            console.log('Specified row, is within range.');
            // check if that prop has more than 0 params.
            var propList = document.getElementById('view-body');
            var test = propList.rows[x].cells[3].innerHTML;
            if(test.includes(0))
            {
                //no params for this property.
                output = "This property has no Parameters";
                console.log('This property has no parameters');
                alert("This property has no parameters.");
                document.getElementById('paramOutput').value = output;
            }
            else
            {
                if(test.includes(1))
                {
                    output = 'Parameter is:'+paramName[0]+", with a value of: "+paramsVals[0];
                }
                if(test.includes(2))
                {
                    output = 'The first Parameter is:'+paramName[0]+', with a value of: '+paramsVals[0]+
                    ',and the second parameter is'+paramName[1]+', with a value of: '+paramsVals[1];

                }
                if(test.includes(3))
                {
                    output = 'The first Parameter is:'+paramName[0]+', with a value of: '+paramsVals[0]+
                    ', and the second parameter is: '+paramName[1]+', with a value of: '+paramsVals[1]+
                    ', The third param is: '+paramName[2]+', With a value of: '+paramsVals[2];
                }
                document.getElementById('paramOutput').value = output;
            }

    }

}
function addPropTable(fileName,y)
{
    // this function increments the number of properties, for the card which a user added a property to.
    var x = document.getElementById("log-body").rows.length;

    var i;
    var table = document.getElementById('log-body');
    var rowVal;
    var compareVal = '<a href=\"'+fileName+'\" download \">'+fileName+'<\"/a>'+'</td>'
    if (y.includes(':'))
    {
        var z = y.split(':');
        if (z[0].length === 0)
        {
            //console.log('');
        }
        else if(z[1].length === 0)
        {
        }
        else {
            for(i =0;i<x;i++)
            {

                rowVal = table.rows[i].cells[0].innerHTML;
                console.log('LOOPING now: '+rowVal);
                    if(rowVal.includes(fileName))
                    {
                        console.log('Found Property');
                        table.rows[i].cells[2].innerHTML++;
                    }
                }
            }
        }

        else {
            console.log("Cannot add property to log table, it is invalid");

        }

}
function createCardFunc(file,fn)
{
    // this function simulates creating a new card. It adds a new row to log body with the user inputted FN and File name.



    if (file.includes(".vcf"))
    {
        var x = file.split('.');
        if (x[0].length === 0)
        {
            //nofilename, error.
            console.log("please include a file name before the .vcf");
            alert("Invalid file name, include name before .vcf");
        }
        else{



            var i;
            var table = document.getElementById('log-body');
            var count = document.getElementById("log-body").rows.length;
            var row;
            var dupName = 0;
            var temp ='<td>'+'<a href='+file+' download ">'+file+'</a>'+'</td>';
            var len1 =temp.length-1;
            console.log("len 1 ="+len1);

            for(i=0;i<count;i++)
            {
                var compareName = table.rows[i].cells[0].innerHTML;
                console.log("len 2 ="+compareName.length);
                if((compareName.includes(x[0])) &&(compareName.length === len1) )
                {
                    dupName = 1;
                }
            }
            if (dupName == 1)
            {
                console.log("File name is already present, change the name");
                alert("Duplicate file name, please change name");
            }
            else {
                row = ("<tr>" +
                '<td>'+'<a href='+file+' download ">'+file+'</a>'+'</td>'+
                '<td>'+fn+'</td>'+
                '<td>'+0+'</td>'+
                "</tr>");
                table.innerHTML += row;


                let option = $('<option>');
                $(option).append(file);
                $('option').attr('id','createdCard');
                $('#vCard-files').append(option);

            }


        }
    }
    else
    {
        // file has invalid extension.
        console.log("Invalid file name. must have .vcf extension");
        alert("Invalid file name, must include .vcf extension");
    }
}
function newProp(name,val)
{
    //this function adds the user input property to the current cards property list

            var row;
            var count = document.getElementById("view-body").rows.length;
            var table = document.getElementById('view-body');
            var changePropNum = document.getElementById('log-body');
            var propName = name;
            var propVal = val;


        //var prop = $('#').find('input[]').val();
            count++;

                row = ("<tr>" +
                '<td>'+count+'</td>'+
                '<td>'+propName+'</td>'+
                '<td>'+propVal+'</td>'+
                '<td>'+0+'</td>'+
                "</tr>");
                table.innerHTML += row;



}
function createDropdown(){
//creates a drop down selection menu for user to select file for full content to display
    var i;
    for( i =0; i<3;i++)
    {
        let option = $('<option>');
        if (i === 0)
        {
            $(option).append('Pick a file');
            $('option').attr('id','default');
            $('#vCard-files').append(option);
        }
        else if (i === 1)
        {
            $(option).append('testCard.vcf');
            $('option').attr('id','f1');
            $('#vCard-files').append(option);
        }
        else if (i === 2)
        {
            $(option).append('testCardMin.vcf');
            $('option').attr('id','f2');
            $('#vCard-files').append(option);
        }
    }

}
function createLogTab()
{
    // this function creates the stub log table with 2 fake files of content
    var i;
    var table = document.getElementById('log-body');
    var row;
    for (i = 0 ;i<2;i++)
    {

        if (i === 0)
        {
            row = ("<tr id=\"testCard.vcf\">"+
            '<td><a href="testCard.vcf" download ">testCard.vcf</a></td>'+
            '<td>Simon perault</td>'+
            '<td>'+10+'</td>'+
            "</tr>");
            table.innerHTML += row;

        }
        if (i === 1)
        {
            row = ("<tr id=\"testCardMin.vcf\">" +
            '<td><a href="testCardMin.vcf" download ">testCardMin.vcf</a></td>'+
            '<td>Simon perault</td>'+
            '<td>'+0+'</td>'+
            "</tr>");
            table.innerHTML += row;
        }
    }
}
function addToTable(x)
{
    //for uploaded files, creates a fake table row for file log panel
    var y = x.split("\\").pop();
    var table = document.getElementById('log-body');
    var len = document.getElementById('log-body').rows.length;
    var compare;
    var row;
    var testName = 0;
    var i;
    for (i=0;i<len;i++)
    {
        compare = table.rows[i].cells[0].innerHTML;
        if(compare.includes(y))
        {
            //name already in use.
            testName =1;
        }
    }
    if (testName === 0)
    {
    row = ("<tr>" +
    '<td><a href="'+y+'" download ">'+y+'</a></td>'+
    '<td>Simon perault</td>'+
    '<td>'+0+'</td>'+
    "</tr>");
    table.innerHTML += row;

    let newSelect = $('<option>');
    $(newSelect).append(y);
    $('newSelect').attr('id','uppFile');
    $('#vCard-files').append(newSelect);
    }
    else {
        console.log("file with same name already present.");
        alert("File with same name present, error.");
    }

}
function displayCard(x)
{
    // displays full content of file.
    var a =  document.getElementById("view-body").rows.length;
    if (a >=1)
    {
        $('#view-body').empty();
    }

    var table = document.getElementById('view-body');
    var row;

    var i;
    if (x === "testCard.vcf")
    {
    for(i=0;i<11;i++)
    {

        // make table for that card.
        if (i === 0)
        {
            row = ("<tr>" +
            '<td>1</td>'+
            '<td>FN</td>'+
            '<td>Simon Perrault</td>'+
            '<td>0</td>'+
            "</tr>");
            table.innerHTML += row;
        }
/////////////////////////////////////////////////////////
        else if(i===1)
        {
            row = ("<tr>" +
            '<td>2</td>'+
            '<td>N</td>'+
            '<td>Perreault, Simon, ing. jr, M.Sc.</td>'+
            '<td>0</td>'+
            "</tr>");
            table.innerHTML += row;
        }
//////////////////////////////////////////////////////
        else if(i===2)
        {
            row = ("<tr>" +
            '<td>3</td>'+
            '<td>BDAY</td>'+
            '<td>Date:--0203</td>'+
            '<td>0</td>'+
            "</tr>");
            table.innerHTML += row;
        }
        //////////////////////////////////////////////////////
        else if(i === 3)
        {
            row = ("<tr>" +
            '<td>4</td>'+
            '<td>ANNIVERSARY</td>'+
            '<td>Date:20090808 Time:143000</td>'+
            '<td>0</td>'+
            "</tr>");
            table.innerHTML += row;
        }
        //////////////////////////////////////////////////////
        else if(i===4)
        {
            row = ("<tr>" +
            '<td>5</td>'+
            '<td>GENDER</td>'+
            '<td>M</td>'+
            '<td>0</td>'+
            "</tr>");
            table.innerHTML += row;

        }
        //////////////////////////////////////////////////////
        else if(i===5)
        {
            row = ("<tr>" +
            '<td>6</td>'+
            '<td>LANG</td>'+
            '<td>fr</td>'+
            '<td>1</td>'+
            "</tr>");
            table.innerHTML += row;

        }
        //////////////////////////////////////////////////////
        else if(i===6)
        {
            row = ("<tr>" +
            '<td>7</td>'+
            '<td>ORG</td>'+
            '<td>Viagenie</td>'+
            '<td>1</td>'+
            "</tr>");
            table.innerHTML += row;

        }
        //////////////////////////////////////////////////////
        else if(i===7)
        {
            row = ("<tr>" +
            '<td>8</td>'+
            '<td>ADR</td>'+
            '<td>Suite D2-630, 2875 Laurier, Quebec, QC, G1V 2M2, Canada</td>'+
            '<td>1</td>'+
            "</tr>");
            table.innerHTML += row;

        }
        //////////////////////////////////////////////////////
        else if(i===8)
        {
            row = ("<tr>" +
            '<td>9</td>'+
            '<td>TEL</td>'+
            '<td>tel:+1-418-656-9254, ext=102</td>'+
            '<td>3</td>'+
            "</tr>");
            table.innerHTML += row;

        }
        //////////////////////////////////////////////////////
        else if(i===9)
        {
            row = ("<tr>" +
            '<td>10</td>'+
            '<td>TEL</td>'+
            '<td>tel:+1-418-262-6501</td>'+
            '<td>1</td>'+
            "</tr>");
            table.innerHTML += row;

        }
        //////////////////////////////////////////////////////


    }

    }
    else if (x === "testCardMin.vcf")
    {

        // makes table for min
        row = ("<tr>" +
        '<td>1</td>'+
        '<td>FN</td>'+
        '<td>Simon Perreault</td>'+
        '<td>0</td>'+
        "</tr>");
        table.innerHTML += row;
    }
    else if (x === "Pick a file")
    {

    }
    else
    {
        var i;
        var fnVal;
        var fileName = x;
        var rowVal;
        var tab = document.getElementById('log-body');
        var x = document.getElementById("log-body").rows.length;
        for(i =0;i<x;i++)
        {

            rowVal = tab.rows[i].cells[0].innerHTML;
            console.log('LOOPING now: '+rowVal);
                if(rowVal.includes(fileName))
                {
                    console.log('Found.');
                    fnVal = tab.rows[i].cells[1].innerHTML;
                }
        }

        // for files added by user or uploaded.
        row = ("<tr>"+
        '<td>1</td>'+
        '<td>FN</td>'+
        '<td>'+fnVal+'</td>'+
        '<td>0</td>'+
        "</tr>");
        table.innerHTML += row;


    }




}
