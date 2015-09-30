#ifndef _SPECTRAL_CLUSTERING_H
#define _SPECTRAL_CLUSTERING_H

#include <QThread>
#include "sample_set.h"

class SpectralClusteringThread : public QThread
{
	Q_OBJECT

public:
	void run(); // Q_DECL_OVERRIDE;
	SpectralClusteringThread(){}
	~SpectralClusteringThread(){}
signals:
	void finish_compute();


};


#endif