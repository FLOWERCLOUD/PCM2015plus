#pragma  once
#include <QThread>
#include "basic_types.h"

namespace SampleManipulation{

	class CaculateNorm :public QThread
	{
		Q_OBJECT
	public:
	static CaculateNorm* getInstance()
		{
			 static CaculateNorm instance;
			return &instance;
		}
		void compute_normal_all(NormalType& baseline);

		void run();
	private:
		CaculateNorm(){

		}

	private:
		static CaculateNorm* instance_ ;
		NormalType baseline_;

	};

}