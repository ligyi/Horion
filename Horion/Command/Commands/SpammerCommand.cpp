#include "SpammerCommand.h"

#include "../../Module/ModuleManager.h"

SpammerCommand::SpammerCommand() : IMCCommand("spammer", "Edit spammer delay/text", "<message/delay/bypass/manual> <string/int/bool>") {
	registerAlias("spam");
}

SpammerCommand::~SpammerCommand() {
}

bool SpammerCommand::execute(std::vector<std::string>* args) {
	assertTrue(g_Data.getLocalPlayer() != nullptr);
	std::string option = args->at(1);
	std::transform(option.begin(), option.end(), option.begin(), ::tolower);
	if (option == "message") {
		std::ostringstream os;
		for (int i = 2; i < args->size(); i++) {
			if (i > 1)
				os << " ";
			os << args->at(i);
		}
		std::string text = os.str().substr(1);
		moduleMgr->getModule<Spammer>()->getMessage() = text;
		clientMessageF("%sSpammer message set to %s%s%s!", GREEN, GRAY, text.c_str(), GREEN);
		return true;
	} else if (option == "delay") {
		int delay = assertInt(args->at(2));
		if (delay < 1) {
			clientMessageF("%sDelay needs to be 1 or more!", RED);
			return true;
		} else {
			moduleMgr->getModule<Spammer>()->getDelay() = delay;
			return true;
		}
	} else if (option == "bypass") {
		std::string data = args->at(2);
		std::transform(data.begin(), data.end(), data.begin(), ::tolower);
		bool state = (data == "true") ? true : false;
		moduleMgr->getModule<Spammer>()->getBypass() = state;
		clientMessageF("%sBypass set to %s%s%s!", GREEN, GRAY, state ? "true" : "false", GREEN);
		return true;
	} else if (option == "manual") {
		int times = assertInt(args->at(2));
		std::ostringstream os;
		for (int i = 3; i < args->size(); i++) {
			if (i > 1)
				os << " ";
			os << args->at(i);
		}
		std::string text = os.str().substr(1);
		Spammer* spammer = moduleMgr->getModule<Spammer>();
		for (int i = 0; i < times; i++) {
			C_TextPacket* textPacket = new C_TextPacket();
			textPacket->message.setText(text + (spammer->getBypass() ? (" | " + Utils::randomString(8)) : ""));
			textPacket->sourceName = *g_Data.getLocalPlayer()->getNameTag();
			textPacket->xboxUserId = TextHolder(std::to_string(g_Data.getLocalPlayer()->getUserId()));
			g_Data.getClientInstance()->loopbackPacketSender->sendToServer(textPacket);
			delete textPacket;
		}
		return true;
	}
	return false;
}
