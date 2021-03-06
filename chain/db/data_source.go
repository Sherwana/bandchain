package db

import (
	"errors"
	"strconv"
	"time"

	"github.com/bandprotocol/bandchain/chain/x/oracle"
	otypes "github.com/bandprotocol/bandchain/chain/x/oracle/types"
	sdk "github.com/cosmos/cosmos-sdk/types"
)

func createDataSource(
	id int64,
	name, description string,
	owner sdk.AccAddress,
	executable []byte,
	blockTime time.Time,
) DataSource {
	return DataSource{
		ID:          id,
		Name:        &name,
		Description: &description,
		Owner:       owner.String(),
		Executable:  executable,
		LastUpdated: blockTime.UnixNano() / int64(time.Millisecond),
	}
}

func (b *BandDB) AddDataSource(
	id int64,
	name, description string,
	owner sdk.AccAddress,
	executable []byte,
	blockTime time.Time,
	blockHeight int64,
	txHash []byte,
) error {
	dataSource := createDataSource(
		id,
		name,
		description,
		owner,
		executable,
		blockTime,
	)
	err := b.tx.Create(&dataSource).Error
	if err != nil {
		return err
	}

	return b.tx.Create(&DataSourceRevision{
		DataSourceID: id,
		Name:         name,
		Timestamp:    blockTime.UnixNano() / int64(time.Millisecond),
		BlockHeight:  blockHeight,
		TxHash:       txHash,
	}).Error
}

func (b *BandDB) handleMsgCreateDataSource(
	txHash []byte,
	msg oracle.MsgCreateDataSource,
	events map[string][]string,
) error {
	ids := events[otypes.EventTypeCreateDataSource+"."+otypes.AttributeKeyID]
	if len(ids) != 1 {
		return errors.New("handleMsgCreateDataSource: cannot find data source id")
	}
	id, err := strconv.ParseInt(ids[0], 10, 64)
	if err != nil {
		return err
	}
	return b.AddDataSource(
		id, msg.Name, msg.Description, msg.Owner, msg.Executable,
		b.ctx.BlockTime(), b.ctx.BlockHeight(), txHash,
	)
}

func (b *BandDB) handleMsgEditDataSource(
	txHash []byte,
	msg oracle.MsgEditDataSource,
) error {
	var dataSource DataSource
	err := b.tx.First(&dataSource, int64(msg.DataSourceID)).Error
	if err != nil {
		return err
	}

	newName := modify(*dataSource.Name, msg.Name)
	newDescription := modify(*dataSource.Description, msg.Description)
	newExecutable := modifyCode(dataSource.Executable, msg.Executable)

	dataSource = createDataSource(
		int64(msg.DataSourceID), newName, newDescription,
		msg.Owner, newExecutable, b.ctx.BlockTime(),
	)

	err = b.tx.Save(&dataSource).Error
	if err != nil {
		return err
	}

	return b.tx.Create(&DataSourceRevision{
		DataSourceID: int64(msg.DataSourceID),
		Name:         newName,
		Timestamp:    b.ctx.BlockTime().UnixNano() / int64(time.Millisecond),
		BlockHeight:  b.ctx.BlockHeight(),
		TxHash:       txHash,
	}).Error
}
