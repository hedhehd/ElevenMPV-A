#ifndef _ELEVENMPV_UTILS_H_
#define _ELEVENMPV_UTILS_H_

#include <kernel.h>
#include <paf.h>

#include "common.h"

using namespace paf;

#define ROUND_UP(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))
#define ROUND_DOWN(x, a) ((x) & ~((a) - 1))
#define IS_ALIGNED(x, a) (((x) & ((a) - 1)) == 0)

#define SCE_APP_EVENT_UNK0					(0x00000003)
#define SCE_APP_EVENT_ON_ACTIVATE			(0x10000001)
#define SCE_APP_EVENT_ON_DEACTIVATE			(0x10000002)
#define SCE_APP_EVENT_UNK1					(0x10000300)
#define SCE_APP_EVENT_REQUEST_QUIT			(0x20000001)
#define SCE_APP_EVENT_UNK2					(0x30000003)

#define FLAG_ELEVENMPVA_IS_FG 1
#define FLAG_ELEVENMPVA_IS_DECODER_USED 2

extern "C" {

	int sceAudioOutSetEffectType(SceInt32 type);

	int sceShellUtilExitToLiveBoard();
}

static const SceUInt32 k_supportedExtNum = 16;
static const SceUInt32 k_supportedCoverExtNum = 4;

static const char *k_supportedExtList[] =
{
	"it",
	"mod",
	"xm",
	"s3m",
	"oma",
	"aa3",
	"at3",
	"ogg",
	"mp3",
	"opus",
	"flac",
	"wav",
	"at9",
	"m4a",
	"aac",
	"webm"
};

static const char *k_supportedCoverExtList[] =
{
	"jpg",
	"jpeg",
	"png",
	"gif"
};

class EMPVAUtils
{
public:

	enum MemState
	{
		MemState_Low,
		MemState_Mid,
		MemState_Full,
	};

	class PleaseWaitThread : public thread::Thread
	{
	public:

		using thread::Thread::Thread;

		SceVoid EntryFunction();

		SceBool withCancel;
		SceBool forceCancel;
		SceBool userCancel;
	};

	class AsyncEnqueue : public paf::thread::JobQueue::Item
	{
	public:

		using thread::JobQueue::Item::Item;

		typedef void(*FinishHandler)();

		~AsyncEnqueue() {}

		SceVoid Run()
		{
			if (!sceKernelPollEventFlag(g_eventFlagUid, FLAG_ELEVENMPVA_IS_FG, SCE_KERNEL_EVF_WAITMODE_AND, SCE_NULL))
				EMPVAUtils::BeginPleaseWait();
			eventHandler(eventId, self, a3, pUserData);
			EMPVAUtils::EndPleaseWait();
		}

		SceVoid Finish()
		{
			if (finishHandler)
				finishHandler();
		}

		static SceVoid JobKiller(thread::JobQueue::Item *job)
		{
			if (job)
				delete job;
		}

		ui::Widget::EventCallback::EventHandler eventHandler;
		FinishHandler finishHandler;
		SceInt32 eventId;
		ui::Widget *self;
		SceInt32 a3;
		ScePVoid pUserData;
	};

	class IPC
	{
	public:

		static SceVoid Enable();

		static SceVoid Disable();

		static SceUInt32 PeekTx();

		static SceVoid SendInfo(WString *title, WString *artist, WString *album, SceInt32 playBtState);

	};

	static SceVoid Init();

	static SceBool IsSupportedExtension(const char *ext);

	static SceBool IsSupportedCoverExtension(const char *ext);

	static SceBool IsRootDevice(const char *path);

	static const char *GetFileExt(const char *filename);

	static SceUInt32 GetHash(const char *name);

	static wchar_t *GetStringWithNum(const char *name, SceUInt32 num);

	static SceUInt32 Downscale(SceInt32 ix, SceInt32 iy, ScePVoid ibuf, SceInt32 ox, SceInt32 oy, ScePVoid obuf);

	static SceInt32 Alphasort(const void *p1, const void *p2);

	static SceBool IsDecoderUsed();

	static SceBool IsSleep();

	static SceInt32 GetDecoderType(const char *path);

	static SceVoid SetPowerTickTask(SceBool enable);

	static SceVoid Exit();

	static SceVoid Activate();

	static SceVoid Deactivate();

	static SceVoid SetMemStatus();

	static SceInt32 GetMemStatus();

	static SceInt32 GetPagemode();

	static SceVoid SetPagemode(SceInt32 mode);

	static SceVoid RunCallbackAsJob(ui::Widget::EventCallback::EventHandler eventHandler, EMPVAUtils::AsyncEnqueue::FinishHandler finishHandler, SceInt32 eventId, ui::Widget *self, SceInt32 a3, ScePVoid pUserData);

	static SceVoid EMPVAUtils::BeginPleaseWait(SceBool withCancel = SCE_FALSE);

	static SceVoid EMPVAUtils::EndPleaseWait();

private:

	static SceVoid PowerTickTask(ScePVoid pUserData);

	static SceVoid AppWatchdogTask(ScePVoid pUserData);

	static SceInt32 PowerCallback(SceInt32 notifyId, SceInt32 notifyCount, SceInt32 powerInfo, ScePVoid common);
};


#endif
