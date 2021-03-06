package consuming

import (

	"github.com/bandprotocol/bandchain/chain/x/oracle"
	sdk "github.com/cosmos/cosmos-sdk/types"
	sdkerrors "github.com/cosmos/cosmos-sdk/types/errors"
	channel "github.com/cosmos/cosmos-sdk/x/ibc/04-channel"
	ibctypes "github.com/cosmos/cosmos-sdk/x/ibc/types"
)

// NewHandler creates the msg handler of this module, as required by Cosmos-SDK standard.
func NewHandler(keeper Keeper) sdk.Handler {
	return func(ctx sdk.Context, msg sdk.Msg) (*sdk.Result, error) {
		ctx = ctx.WithEventManager(sdk.NewEventManager())
		switch msg := msg.(type) {
		case MsgRequestData:
			sourceChannelEnd, found := keeper.ChannelKeeper.GetChannel(ctx, "consuming", msg.SourceChannel)
			if !found {
				return nil, sdkerrors.Wrapf(
					sdkerrors.ErrUnknownRequest,
					"unknown channel %s port consuming",
					msg.SourceChannel,
				)
			}
			destinationPort := sourceChannelEnd.Counterparty.PortID
			destinationChannel := sourceChannelEnd.Counterparty.ChannelID
			sequence, found := keeper.ChannelKeeper.GetNextSequenceSend(
				ctx, "consuming", msg.SourceChannel,
			)
			if !found {
				return nil, sdkerrors.Wrapf(
					sdkerrors.ErrUnknownRequest,
					"unknown sequence number for channel %s port oracle",
					msg.SourceChannel,
				)
			}
			packet := oracle.NewOracleRequestPacketData(
				msg.ClientID, msg.OracleScriptID, msg.Calldata,
				msg.AskCount, msg.MinCount,
			)
			chanCap, ok := keeper.ScopedKeeper.GetCapability(ctx, ibctypes.ChannelCapabilityPath("consuming", msg.SourceChannel))
			if !ok {
				return nil, sdkerrors.Wrap(channel.ErrChannelCapabilityNotFound, "module does not own channel capability")
			}
			err := keeper.ChannelKeeper.SendPacket(ctx, chanCap, channel.NewPacket(packet.GetBytes(),
				sequence, "consuming", msg.SourceChannel, destinationPort, destinationChannel,
				10000000, 1746114147000000000, // Arbitrarily high timeout and timeout stamp for now
			))
			if err != nil {
				return nil, err
			}
			return &sdk.Result{Events: ctx.EventManager().Events().ToABCIEvents()}, nil
		default:
			return nil, sdkerrors.Wrapf(sdkerrors.ErrUnknownRequest, "unrecognized %s message type: %T", ModuleName, msg)
		}
	}
}
