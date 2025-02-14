#ifndef CSV_RENDER_TERRAINTEXTUREMODE_H
#define CSV_RENDER_TERRAINTEXTUREMODE_H

#include "editmode.hpp"

#include <string>
#include <memory>

#include <QWidget>
#include <QEvent>

#ifndef Q_MOC_RUN
#include "../../model/doc/document.hpp"
#include "../../model/world/idtable.hpp"
#include "../../model/world/columnimp.hpp"
#include "../widget/brushshapes.hpp"
#include "brushdraw.hpp"
#endif

#include "terrainselection.hpp"

namespace osg
{
    class Group;
}

namespace CSVWidget
{
    class SceneToolTextureBrush;
}

namespace CSVRender
{
    class TerrainTextureMode : public EditMode
    {
        Q_OBJECT

        public:

            enum InteractionType
            {
                InteractionType_PrimaryEdit,
                InteractionType_PrimarySelect,
                InteractionType_SecondaryEdit,
                InteractionType_SecondarySelect,
                InteractionType_None
            };

            /// \brief Editmode for terrain texture grid
            TerrainTextureMode(WorldspaceWidget*, osg::Group* parentNode, QWidget* parent = nullptr);

            void primaryOpenPressed (const WorldspaceHitResult& hit) override;

            /// \brief Create single command for one-click texture editing
            void primaryEditPressed (const WorldspaceHitResult& hit) override;

            /// \brief Open brush settings window
            void primarySelectPressed(const WorldspaceHitResult&) override;

            void secondarySelectPressed(const WorldspaceHitResult&) override;

            void activate(CSVWidget::SceneToolbar*) override;
            void deactivate(CSVWidget::SceneToolbar*) override;

            /// \brief Start texture editing command macro
            bool primaryEditStartDrag (const QPoint& pos) override;

            bool secondaryEditStartDrag (const QPoint& pos) override;
            bool primarySelectStartDrag (const QPoint& pos) override;
            bool secondarySelectStartDrag (const QPoint& pos) override;

            /// \brief Handle texture edit behavior during dragging
            void drag (const QPoint& pos, int diffX, int diffY, double speedFactor) override;

            /// \brief End texture editing command macro
            void dragCompleted(const QPoint& pos) override;

            void dragAborted() override;
            void dragWheel (int diff, double speedFactor) override;
            void dragMoveEvent (QDragMoveEvent *event) override;

            void mouseMoveEvent (QMouseEvent *event) override;

            std::shared_ptr<TerrainSelection> getTerrainSelection();

        private:
            /// \brief Handle brush mechanics, maths regarding worldspace hit etc.
            void editTerrainTextureGrid (const WorldspaceHitResult& hit);

            /// \brief Check if global selection coordinate belongs to cell in view
            bool isInCellSelection(int globalSelectionX, int globalSelectionY);

            /// \brief Handle brush mechanics for texture selection
            void selectTerrainTextures (const std::pair<int, int>& texCoords, unsigned char selectMode);

            /// \brief Push texture edits to command macro
            void pushEditToCommand (CSMWorld::LandTexturesColumn::DataType& newLandGrid, CSMDoc::Document& document,
                CSMWorld::IdTable& landTable, std::string cellId);

            /// \brief Create new land texture record from texture asset
            void createTexture(std::string textureFileName);

            /// \brief Create new cell and land if needed
            bool allowLandTextureEditing(std::string textureFileName);

            std::string mCellId;
            std::string mBrushTexture;
            int mBrushSize;
            CSVWidget::BrushShape mBrushShape;
            std::unique_ptr<BrushDraw> mBrushDraw;
            std::vector<std::pair<int, int>> mCustomBrushShape;
            CSVWidget::SceneToolTextureBrush *mTextureBrushScenetool;
            int mDragMode;
            osg::Group* mParentNode;
            bool mIsEditing;
            std::shared_ptr<TerrainSelection> mTerrainTextureSelection;

            const int cellSize {ESM::Land::REAL_SIZE};
            const int landTextureSize {ESM::Land::LAND_TEXTURE_SIZE};

        signals:
            void passBrushTexture(std::string brushTexture);

        public slots:
            void handleDropEvent(QDropEvent *event);
            void setBrushSize(int brushSize);
            void setBrushShape(CSVWidget::BrushShape brushShape);
            void setBrushTexture(std::string brushShape);
    };
}


#endif
