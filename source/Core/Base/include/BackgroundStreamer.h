/*
 * BackgroundStreamer.h
 *
 *  Created on: 17-Sep-2013
 *      Author: obhi
 */

#ifndef BACKGROUNDSTREAMER_H_
#define BACKGROUNDSTREAMER_H_

#include <NexBase.h>
#include <Singleton.h>

namespace nextar {

enum class StreamResult {
	LOAD_SUCCESS,
	LOAD_TRY_AGAIN,
	LOAD_FAILED,
	SAVE_SUCCESS,
	SAVE_TRY_AGAIN,
	SAVE_FAILED,
};

struct Streamable;
struct StreamHandler;
struct StreamRequest;

struct Streamable {

	virtual ~Streamable() {
	}

	/* Background loading, can own and modify @req */
	virtual void AsyncLoad(StreamRequest* req) = 0;
	/* Background unloading, can own and modify @req */
	virtual void AsyncSave(StreamRequest* req) = 0;
};

class StreamHandler {
public:
	/** Handles load completion */
	virtual void NotifyLoaded(StreamRequest*) = 0;
	virtual void NotifySaved(StreamRequest*) = 0;

protected:
	~StreamHandler() {}
};

struct StreamRequest {
	enum StreamRequestFlags {

		// Indicates this request is of type AssetStreamRequest 
		ASSET_STREAM_REQUEST = 1 << 0,
		// Indicates this request should be deleted, when completed 
		AUTO_DELETE_REQUEST = 1 << 1,
		// Indicates this request was handled and no further processing is required. 
		COMPLETED = 1 << 2,
		// Indicates AsyncUnload and NotifyUnloaded are to be called
		REQUEST_SAVE = 1 << 4,

		LAST_FLAG = 1 << 5,
	};

	uint16 flags;
	uint16 returnCode;
	Streamable* streamedObject;
	StreamHandler* streamHandler;

	inline Streamable* GetStreamedObject() {
		return streamedObject;
	}

	StreamRequest(Streamable* streamed = 0, uint16 streamFlags = 0) :
			streamedObject(streamed), flags(streamFlags), returnCode(0), streamHandler(
					0) {
	}

};

// to extend this class to support external streaming as well as component streaming
class _NexBaseAPI BackgroundStreamer: public Singleton<BackgroundStreamer>,
		public AllocGeneral {

public:

	BackgroundStreamer();
	virtual ~BackgroundStreamer();

	virtual void AddRequest(StreamRequest*) = 0;
};

} /* namespace nextar */
#endif /* BACKGROUNDSTREAMER_H_ */
