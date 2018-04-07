#ifndef BABLE_LINUX_SERIALIZERINTERFACE_H
#define BABLE_LINUX_SERIALIZERINTERFACE_H

/** Interface to implement serializer/deserializer. Used to convert data from one format to another. */
class SerializerInterface {

public:

  virtual void* serialize() = 0;
  virtual void* unserialize() = 0;

private:

};

#endif //BABLE_LINUX_SERIALIZERINTERFACE_H
