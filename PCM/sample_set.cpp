#include "sample_set.h"

void SampleSet::push_back( Sample* new_sample )
{
	if (new_sample != nullptr)
	{
		set_.push_back(new_sample);
	}

}

void SampleSet::clear()
{
	while( set_.empty() == false )
	{
		delete set_.back();
		set_.pop_back();
	}
}
