using Newtonsoft.Json.Linq;

/// <summary>
/// Helper class used to encapsulate message data.
/// </summary>
class HSIFMsg
{
    #region properties

    // Message properties
    public string jsonData;
    public string toId;
    public string fromId;

    #endregion
    
    // Initialize with current endpoint id, sending endpoint id, and json data
    public HSIFMsg(string toId, string fromId, string jsonData)
    {
        this.toId = toId;
        this.fromId = fromId;
        this.jsonData = jsonData;
    }

    // Initialize from json string
    public HSIFMsg(string jsonStr)
    {
        Serialize(jsonStr);
    }
    
    // Retrieves json string of HSIFMsg instance
    public string GetJson()
    {
        string json = "{ \"type\" : \"message\", \"to\" : \"" + this.toId + "\", \"from\" : \"" + this.fromId + "\", \"data\" : " + this.jsonData + " }";
        return json;
    }

    // Serialize object from json string
    public void Serialize(string jsonStr)
    {
        JObject jObject = JObject.Parse(jsonStr);
        toId = (string)jObject["to"];
        fromId = (string)jObject["from"];
        jsonData = (string)jObject["data"].ToString();
    }
}
