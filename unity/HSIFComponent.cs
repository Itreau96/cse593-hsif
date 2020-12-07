using System.Collections.Concurrent;
using System.Collections.Generic;
using UnityEngine;
using System.Net.Sockets;
using System.IO;
using System;
using System.Text;
using System.Threading;
using System.Linq;
using System.Collections;

/// <summary>
/// Abstract base class for HSIF component. 
/// Handles socket configuration to HSIF server and 
/// exposes interface for managing messaging between
/// server and client.
/// </summary>
public abstract class HSIFComponent : MonoBehaviour
{
    #region Properties

    // Configurable endpoint connection settings. 
    // Ensure your endpoint is setup to receive 
    // serial output data using the port and ip below.
    public string host = "localhost";
    public int port = 8888;
    public string endpointId = "";

    // Socket communication resources.
    protected TcpClient hsifSocket;
    protected NetworkStream dataStream;

    // Threading management resources.
    private Thread receiveThread;
    protected ConcurrentQueue<string> msgInbox;
    protected ConcurrentQueue<string> msgOutbox;

    #endregion

    #region Constants

    const char DELIMITER = '\r';

    #endregion

    // Start is called before the first frame update
    void Start()
    {
        try
        {
            // Initialize message queues
            msgInbox = new ConcurrentQueue<string>();
            msgOutbox = new ConcurrentQueue<string>();
            RegisterEndpoint();
            receiveThread = new Thread(new ThreadStart(ListenForData))
            {
                IsBackground = true
            };
            receiveThread.Start();
        }
        catch (Exception e)
        {
            Debug.Log("Socket error:" + e); // Catch exceptions setting up the port communication
        }
    }

    // Helper function used to register endpoint with identifier so messages can be received
    void RegisterEndpoint()
    {
        // Create registration message
        string message = "{ \"type\": \"registration\", \"value\" : \"" + endpointId + "\" }";
        // Add to outbox
        msgOutbox.Enqueue(message);
    }

    // Thread used to handle data IO service requests between client and server.
    void ListenForData()
    {
        try
        {
            // Initialize socket binding.
            hsifSocket = new TcpClient(host, port);
            dataStream = hsifSocket.GetStream();
            var largeBuffer = new List<byte>();
            int msgSize = 0;
            byte delim = (byte) DELIMITER;
            ArrayList listenList = new ArrayList() { hsifSocket.Client };
            ArrayList writeList = new ArrayList();
            Debug.Log("HSIF socket initialized successfully: host " + host + ", port " + port);
            // Something happens in-scene once per frame during the 
            // update frame. Using the data read/write booleans ensure
            // data communication is synchronized.
            byte[] myReadBuffer = new byte[1024];
            var ms = new MemoryStream();
            while (true)
            {
                // Add server connection to listen list by default.
                listenList.Add(hsifSocket.Client);
                // Only add to write list if data available to be written.
                if (!msgOutbox.IsEmpty)
                {
                    writeList = new ArrayList() { hsifSocket.Client };
                }

                // Determine sockets to read from and write to.
                Socket.Select(listenList, writeList, null, 1000);

                // Remove dead space to simplify parsing
                if (listenList.Contains(hsifSocket.Client))
                {
                    // Get chunk
                    int numberOfBytesRead = dataStream.Read(myReadBuffer, 0, myReadBuffer.Length);
                    byte[] packet = myReadBuffer.SubArray(0, numberOfBytesRead);
                    // Loop while packet has contents
                    while (packet.Length != 0)
                    {
                        // If not in middle of read...
                        if (msgSize == 0)
                        {
                            if (packet.Contains(delim))
                            {
                                // Extract size from message
                                var msgPartial = Encoding.UTF8.GetString(packet);
                                var msgSplit = msgPartial.Split(new[] { DELIMITER }, 2, StringSplitOptions.None);
                                msgSize = int.Parse(msgSplit[0]);

                                // Resize packet
                                packet = Encoding.UTF8.GetBytes(msgSplit[1]);
                            }
                            else
                            {
                                throw new Exception("The packet was malformed!");
                            }
                        }
                        else
                        {
                            // If more than one message in packet, loop through and extract
                            if (packet.Length >= msgSize)
                            {
                                // Extract current message from packet
                                var remainder = packet.SubArray(0, msgSize);
                                largeBuffer.AddRange(packet);
                                var currentMsg = Encoding.UTF8.GetString(largeBuffer.ToArray());
                                msgInbox.Enqueue(currentMsg);
                                packet = packet.SubArray(msgSize, packet.Length - msgSize);
                                largeBuffer.Clear();
                                msgSize = 0;
                            }
                            // If message size larger than packet, add to buffer and continue
                            else
                            {
                                largeBuffer.AddRange(packet);
                                msgSize -= packet.Length;
                                packet = new byte[0];
                            }
                        }
                    }
                }
                // If outgoing messages, send all before looping again
                if (writeList.Contains(hsifSocket.Client))
                {
                    // Send output data if output is ready
                    while (msgOutbox.Count != 0)
                    {
                        if (msgOutbox.TryDequeue(out string currentMsg))
                        {
                            var finalMsg = Encoding.UTF8.GetBytes(currentMsg).Length.ToString() + DELIMITER + currentMsg;
                            var msgBytes = Encoding.UTF8.GetBytes(finalMsg);
                            dataStream.Write(msgBytes, 0, msgBytes.Length);
                        }
                        else
                        {
                            Debug.Log("Problem retreiving message from outbox!");
                        }
                    }
                }
            }
        }
        catch (SocketException socketException)
        {
            Debug.Log("Socket exception occurred: " + socketException);
        }
    }

    // Update is called once per frame and is the only time data is processed
    public virtual void Update()
    {
        try
        {
            // If message received, execute callback function
            while (!msgInbox.IsEmpty)
            {
                if (msgInbox.TryDequeue(out string currentMsg))
                {
                    DataReceived(currentMsg);
                }
                else
                {
                    throw new Exception("Could not read incoming message from buffer.");
                }
            }
        }
        catch (Exception e)
        {
            Debug.Log("Exception occurred during update: " + e);
        }
    }

    /// <summary>
    /// User defined message handler. Called once per frame.
    /// </summary>
    /// <returns>String data to be sent back to HSIF server.</returns>
    protected abstract void DataReceived(string data);

    // Clean up port connection
    void OnDestroy()
    {
        dataStream.Close();
        hsifSocket.Close();
        Debug.Log("HSIF socket connection closed.");
    }
}

/// <summary>
/// Helper class extension used to provide subarray functionality.
/// </summary>
public static class Extensions
{
    public static T[] SubArray<T>(this T[] array, int offset, int length)
    {
        T[] result = new T[length];
        Array.Copy(array, offset, result, 0, length);
        return result;
    }
}
