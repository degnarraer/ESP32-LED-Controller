#ifndef DataTypes_H
#define DataTypes_H 

class NamedItem
{
  public:
	NamedItem(String Title): m_Title(Title){}
	virtual ~NamedItem(){}
    String GetTitle()
    { 
      return m_Title;
    }
  private:
    String m_Title;
};

enum Transciever_T
{
	Transciever_None,
	Transciever_TX,
	Transciever_RX,
	Transciever_TXRX
};

enum DataType_t
{
  DataType_Int16_t,
  DataType_Int32_t,
  DataType_Uint16_t,
  DataType_Uint32_t,
  DataType_String,
  DataType_Float,
};

static const char* DataTypeStrings[] =
{
  "Int16_t",
  "Int32_t",
  "Uint16_t",
  "Uint32_t",
  "String",
  "Float"
};

struct DataItemConfig_t
{
  String Name;
  DataType_t DataType;
  size_t Count;
  Transciever_T TransceiverConfig;
  
};

struct DataItem_t
{
  String Name;
  DataType_t DataType;
  size_t Count;
  QueueHandle_t QueueHandle;
  Transciever_T TransceiverConfig;
  void* Object;
};

#endif
