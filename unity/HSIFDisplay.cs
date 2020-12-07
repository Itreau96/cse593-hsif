using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net.Sockets;
using TMPro;
using System.IO;
using System;
using System.Text;
using System.Threading;

/// <summary>
/// HSIF component used to display data
/// retrieved by Rpi. 
/// </summary>
public class HSIFDisplay : HSIFComponent
{
    #region Properties

    // Reference to text display component
    public TextMeshPro textOutput;
    // Id of endpoint to send to
    public string sendTo;

    #endregion

    // Overriden data handler. The display component uses it to change display 
    protected override void DataReceived(string data)
    {
        // Format and display input data
        HSIFMsg input = new HSIFMsg(data);
        textOutput.text =  "Output from Raspberry Pi: " + '\n' + input.jsonData;
        Debug.Log("Temperature data received { " + data + " }");
        // Create confirmation message
        HSIFMsg message = new HSIFMsg(sendTo, endpointId, "{ \"ack\" : \"Message received!\" }");
        // Add to outbox
        msgOutbox.Enqueue(message.GetJson());
    }
}
