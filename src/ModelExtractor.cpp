#include "hiberlite.h"

namespace hiberlite{

ModelExtractor::ModelExtractor() : AVisitor<ExtractModel>(&actor, MODEL)
{}

Model ExtractModel::getModel() {
	return model;
}

Model ModelExtractor::getModel() {
	return actor.getModel();
}

} //namespace hiberlite
