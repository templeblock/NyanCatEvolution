#include "DnaPolygon.h"
#include <algorithm>
#include <iostream>
#include "settings.h"
#include "tools.h"

// A new DnaPolygon is initiated around a random point on canvas.
DnaPolygon::DnaPolygon()
{
	points = new std::vector<DnaPoint>;
	// Create polygon around a random point origin.
	for (int i = 0; i < settings::PointsPerPolygonMin; i++) {
		DnaPoint origin;
		origin.Random();

		int _x = origin.x + tools::GetRandomNumber(
			-settings::PolygonInitRange, settings::PolygonInitRange + 1);
		int _y = origin.y + tools::GetRandomNumber(
			-settings::PolygonInitRange, settings::PolygonInitRange + 1);

		DnaPoint* point = new DnaPoint;
		point->x = std::min(std::max(0, _x), settings::MaxWidth);
		point->y = std::min(std::max(0, _y), settings::MaxHeight);
		(this->points)->push_back(*point);
	};
	// Add a brush for polygon.
	brush = new DnaBrush;
}

DnaPolygon::DnaPolygon(const DnaPolygon& polygon)
{
	// Copy points.
	points = new std::vector < DnaPoint >;
	for (auto point : *(polygon.points)) {
		points->push_back(point);
	};
	// Copy brush.
	brush = polygon.brush->Clone();
}

DnaPolygon::~DnaPolygon()
{
	delete points;
	delete brush;
}

DnaPolygon& DnaPolygon::operator=(const DnaPolygon& polygon)
{
	if (this != &polygon) {
		// Copy points.
		delete points;
		points = new std::vector < DnaPoint >;
		for (auto point : *(polygon.points)) {
			points->push_back(point);
		};
		// Copy brush.
		delete brush;
		brush = polygon.brush->Clone();
	};
	return *this;
}

int DnaPolygon::Count() const { return points->size(); }

DnaPolygon* DnaPolygon::Clone()
{
	DnaPolygon * polygon = new DnaPolygon;
	// Copy points.
	delete polygon->points;
	polygon->points = new std::vector < DnaPoint >;
	for (auto p : *points) {
		(polygon->points)->push_back(p);
	};
	// Copy brush.
	polygon->brush = brush->Clone();
	return polygon;
}

// DnaPolygon can mutate in the following ways:
// 1. Add a point, on polygon's edge to avoid destroying the polygon.
// 2. Remove a point.
// 3. Move around a point.
// 4. Change the brush color.
void DnaPolygon::Mutate(DnaCanvas& canvas)
{
	int prob = tools::GetRandomNumber(0, settings::PolygonMutationRateBase);
	int cut1 = settings::PolygonAddPointRate;
	int cut2 = cut1 + settings::PolygonRemovePointRate;
	int cut3 = cut2 + settings::PolygonMutatePointRate;
	int cut4 = cut3 + settings::PolygonMutateBrushRate;

	if (prob < cut1) { AddPoint(canvas); }
	else if (prob < cut2) { RemovePoint(canvas); }
	else if (prob < cut3) { MutatePoint(canvas); }
	else if (prob < cut4) { MutateBrush(canvas); };
}

void DnaPolygon::AddPoint(DnaCanvas& canvas)
{
	if (this->Count() < settings::PointsPerPolygonMax) {
		if (canvas.PointCount() < settings::PointsPerCanvasMax) {
			// Add new point between two neighbouring points to keep a convex shape
			// and minimize the change in rendered image.
			int index = tools::GetRandomNumber(1, points->size());
			DnaPoint point(
				((*points)[index - 1].x + (*points)[index].x) / 2,
				((*points)[index - 1].y + (*points)[index].y) / 2);
			points->insert(points->begin() + index, point);
			canvas.SetDirty();
		};
	};
}

void DnaPolygon::RemovePoint(DnaCanvas& canvas)
{
	if (this->Count() > settings::PointsPerPolygonMin) {
		if (canvas.PointCount() > settings::PointsPerCanvasMin) {
			int index = tools::GetRandomNumber(0, points->size());
			points->erase(points->begin() + index);
			canvas.SetDirty();
		};
	};
}

void DnaPolygon::MutatePoint(DnaCanvas& canvas)
{
	int index = tools::GetRandomNumber(0, points->size());
	(*points)[index].Mutate(canvas);
}

void DnaPolygon::MutateBrush(DnaCanvas& canvas)
{
	brush->Mutate(canvas);
}