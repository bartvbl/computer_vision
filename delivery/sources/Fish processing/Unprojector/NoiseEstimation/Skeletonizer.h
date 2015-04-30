#pragma once
class Skeletonizer
{
public:
	Skeletonizer(void);
	~Skeletonizer(void);

	static void process(std::string src, std::string name);
};

