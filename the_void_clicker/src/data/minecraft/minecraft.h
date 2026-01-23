#pragma once


namespace Java {
	class CMinecraft {
	private:
		jobject instance;
	public:
		CMinecraft(jobject obj) : instance(obj) {}

		jobject GetInstance();
		CMinecraft GetMinecraft();
	};
}