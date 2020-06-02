module Styles = {
  open Css;

  let vFlex = style([display(`flex), flexDirection(`row), alignItems(`center)]);

  let logo = style([width(`px(50)), marginRight(`px(10))]);
  let logoSmall = style([width(`px(20))]);

  let fillLeft = style([marginLeft(`auto)]);

  let topPartWrapper =
    style([
      width(`percent(100.0)),
      display(`flex),
      flexDirection(`column),
      backgroundColor(Colors.white),
      borderRadius(`px(4)),
      padding2(~v=`px(35), ~h=`px(30)),
      boxShadow(Shadow.box(~x=`zero, ~y=`px(2), ~blur=`px(8), Css.rgba(0, 0, 0, 0.08))),
    ]);

  let fullWidth = dir => style([width(`percent(100.0)), display(`flex), flexDirection(dir)]);

  let seperatedLine =
    style([
      width(`px(13)),
      height(`px(1)),
      marginLeft(`px(10)),
      marginRight(`px(10)),
      backgroundColor(Colors.blueGray2),
    ]);

  let longLine =
    style([
      width(`percent(100.)),
      height(`px(2)),
      backgroundColor(Colors.blueGray2),
      marginTop(`px(30)),
      marginBottom(`px(45)),
    ]);

  let underline = style([textDecoration(`underline), color(Colors.gray7)]);
};

type value_row_t =
  | VAddress(Address.t)
  | VValidatorAddress(Address.t)
  | VText(string)
  | VDetail(string)
  | VExtLink(string)
  | VCode(string);

let kvRow = (k, v: value_row_t) => {
  <Row alignItems=`flexStart>
    <Col size=1.>
      <div className={Styles.fullWidth(`row)}> <Text value=k weight=Text.Thin /> </div>
    </Col>
    <Col size=1. justifyContent=Col.Center alignItems=Col.End>
      <div className={Styles.fullWidth(`row)}>
        <div className=Styles.fillLeft />
        {switch (v) {
         | VAddress(address) => <AddressRender address />
         | VValidatorAddress(address) => <AddressRender address validator=true clickable=false />
         | VText(value) => <Text value nowrap=true />
         | VDetail(value) => <Text value align=Text.Right />
         | VExtLink(value) =>
           <a href=value target="_blank" rel="noopener">
             <div className=Styles.underline> <Text value nowrap=true /> </div>
           </a>
         | VCode(value) => <Text value code=true nowrap=true />
         }}
      </div>
    </Col>
  </Row>;
};

module Uptime = {
  [@react.component]
  let make = (~consensusAddress) =>
    {
      let%Sub uptimeOpt = ValidatorSub.getUptime(consensusAddress);

      switch (uptimeOpt) {
      | Some(uptime) => kvRow("UPTIME", VCode(uptime->Format.fPercent)) |> Sub.resolve
      | None => kvRow("UPTIME", VText("N/A")) |> Sub.resolve
      };
    }
    |> Sub.default(_, React.null);
};

[@react.component]
let make = (~address, ~hashtag: Route.validator_tab_t) =>
  {
    let validatorSub = ValidatorSub.get(address);
    let bondedTokenCountSub = ValidatorSub.getTotalBondedAmount();
    let%Sub validator = validatorSub;
    let%Sub rawBondedTokenCount = bondedTokenCountSub;

    let bondedTokenCount = rawBondedTokenCount.amount;

    <>
      <Row justify=Row.Between>
        <Col>
          <div className=Styles.vFlex>
            <img src=Images.validatorLogo className=Styles.logo />
            <Text
              value="VALIDATOR DETAILS"
              weight=Text.Medium
              size=Text.Md
              spacing={Text.Em(0.06)}
              height={Text.Px(15)}
              nowrap=true
              color=Colors.gray7
              block=true
            />
            <div className=Styles.seperatedLine />
            <Text
              value={validator.isActive ? "ACTIVE" : "INACTIVE"}
              size=Text.Md
              weight=Text.Thin
              spacing={Text.Em(0.06)}
              color=Colors.gray7
              nowrap=true
            />
            <HSpacing size=Spacing.md />
            <img
              src={validator.isActive ? Images.activeValidatorLogo : Images.inactiveValidatorLogo}
              className=Styles.logoSmall
            />
          </div>
        </Col>
      </Row>
      <VSpacing size=Spacing.xl />
      <div className=Styles.vFlex>
        <Text value={validator.moniker} size=Text.Xxl weight=Text.Bold nowrap=true />
      </div>
      <VSpacing size=Spacing.xl />
      <ValidatorStakingInfo validatorAddress={validator.operatorAddress} />
      <VSpacing size=Spacing.md />
      <div className=Styles.topPartWrapper>
        <Text value="INFORMATION" size=Text.Lg weight=Text.Semibold />
        <VSpacing size=Spacing.lg />
        {kvRow("OPERATOR ADDRESS", VValidatorAddress(address))}
        <VSpacing size=Spacing.lg />
        {kvRow("ADDRESS", VAddress(address))}
        <VSpacing size=Spacing.lg />
        {kvRow(
           "VOTING POWER",
           VCode(
             (bondedTokenCount > 0. ? validator.votingPower *. 100. /. bondedTokenCount : 0.)
             ->Format.fPretty
             ++ "% ("
             ++ (validator.votingPower /. 1e6 |> Format.fPretty)
             ++ " BAND)",
           ),
         )}
        <VSpacing size=Spacing.lg />
        {kvRow("COMMISSION", VCode(validator.commission->Format.fPercent))}
        <VSpacing size=Spacing.lg />
        {kvRow("BONDED HEIGHT", VCode(validator.bondedHeight->Format.iPretty))}
        <VSpacing size=Spacing.lg />
        <Uptime consensusAddress={validator.consensusAddress} />
        <VSpacing size=Spacing.lg />
        {kvRow("WEBSITE", VExtLink(validator.website))}
        <VSpacing size=Spacing.lg />
        {kvRow("DETAILS", VDetail(validator.details))}
      </div>
      // <div className=Styles.longLine />
      // <div className={Styles.fullWidth(`row)}>
      //   <Col size=1.>
      //     <Text value="NODE STATUS" size=Text.Lg weight=Text.Semibold />
      //     <VSpacing size=Spacing.lg />
      //     {kvRow("UPTIME", VCode(validator.nodeStatus.uptime->Format.fPretty ++ "%"))}
      //     <VSpacing size=Spacing.lg />
      //     {kvRow(
      //        "AVG. RESPONSE TIME",
      //        VCode(
      //          validator.avgResponseTime->Format.iPretty
      //          ++ (validator.avgResponseTime <= 1 ? " block" : " blocks"),
      //        ),
      //      )}
      //   </Col>
      //   <HSpacing size=Spacing.lg />
      //   <Col size=1.>
      //     <Text value="REQUEST RESPONSE" size=Text.Lg weight=Text.Semibold />
      //     <VSpacing size=Spacing.lg />
      //     {kvRow("COMPLETED REQUESTS", VCode(validator.completedRequestCount->Format.iPretty))}
      //     <VSpacing size=Spacing.lg />
      //     {kvRow("MISSED REQUESTS", VCode(validator.missedRequestCount->Format.iPretty))}
      //   </Col>
      // </div>
      <VSpacing size=Spacing.md />
      <Tab
        tabs=[|
          {
            name: "PROPOSED BLOCKS",
            route: Route.ValidatorIndexPage(address, Route.ProposedBlocks),
          },
          {name: "DELEGATORS", route: Route.ValidatorIndexPage(address, Route.Delegators)},
          {name: "REPORTS", route: Route.ValidatorIndexPage(address, Route.Reports)},
        |]
        currentRoute={Route.ValidatorIndexPage(address, hashtag)}>
        {switch (hashtag) {
         | ProposedBlocks => <ProposedBlocksTable consensusAddress={validator.consensusAddress} />
         | Delegators => <DelegatorsTable address />
         | Reports => <ReportsTable address />
         }}
      </Tab>
    </>
    |> Sub.resolve;
  }
  |> Sub.default(_, React.null);
