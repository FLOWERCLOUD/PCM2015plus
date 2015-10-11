#pragma  once
#include "basic_types.h"
#include "multiway_propagation.h"
#include <QThread>

class PropagateThread :public QThread
{
	Q_OBJECT
public:
	static PropagateThread& getInstance()
	{
		static PropagateThread instance_;
		return instance_;
	}
	void setParameters( IndexType _selectedSmpIdx, IndexType _centerframe, IndexType _front_endframe,
		IndexType _back_endframe, bool	_is_propagate_front_, bool   _is_propagate_back_)
	{
		selectedSmpIdx_ = _selectedSmpIdx;

		centerframe_ = _centerframe;
		front_endframe_ = _front_endframe;
		back_endframe_ = _back_endframe;
		is_propagate_front_ = is_propagate_front_;
		is_propagate_back_ = _is_propagate_back_;

	}
	void run();


public:

	PropagateThread():dp_(DualwayPropagation::get_instance())
	{

	}
	void split_twoAjacent_graph_next( DualwayPropagation& dp ,IndexType srFrame, IndexType tgFrame);
	void split_twoAjacent_graph_prev( DualwayPropagation& dp ,IndexType srFrame, IndexType tgFrame);
	void copyLabelBucket(vector<HLabel*>& leftLabels, const vector<HLabel*>& oriLabels);
	IndexType checkNextLabelBucket(map<IndexType,HVertex*>& edgePs, map<IndexType,HVertex*>& edgePsCor);
	void determinateUnmarkPoints(IndexType cFrame,map<IndexType,HVertex*>& unMarkPs,vector<HLabel*> oriLabelBucket,IndexType nodeId,IndexType newLabe,IndexType tgSize);
	void minDistBeTwoParts(IndexType cFrame, map<IndexType,HVertex*>& fPart,map<IndexType,HVertex*>& sPart, ScalarType& misDis);
	void checkPsNewLabelParentPtr(vector<HLabel*> oriLabelBucket,IndexType labParSize);
	ScalarType p2PatchAvgDis(IndexType cFrame, PointType& pCoor,map<IndexType,HVertex*> parthPs);
	bool checkPrevLabelBucket(map<IndexType,HVertex*>& edgePs, map<IndexType,HVertex*>& edgePsCor, IndexType& isSplit);
	void visualCosegmentation(char *labels_file);
	void getEdgePoints(  HLabel*  p_labell,  HLabel*  p_labelr ,IndexType _frameId ,map<IndexType, map<IndexType ,HVertex*> >& edgePoints );
	void propagateLabel2Orignal();
	void propagateLabel2Orignal(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_ori , IndexType _labelNum);
	void visualAllvtx();
	IndexType selectedSmpIdx_;

	IndexType centerframe_;
	IndexType front_endframe_;
	IndexType back_endframe_;
	bool	is_propagate_front_;
	bool    is_propagate_back_;

	DualwayPropagation& dp_;

};
